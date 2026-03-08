/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include <algorithm>
#include <cctype>
#include <optional>
#include <stack>
#include <string>

#include "eml_parser.h"

#include "charset_converter.h"
#include "error_tags.h"
#include "convert_base.h"
#include "mail_elements.h"
#include "data_source.h"
#include "document_elements.h"
#include <iostream>
#include "log_entry.h"
#include "log_scope.h"
#include <mailio/message.hpp>
#include <mailio/mime.hpp>
#include "make_error.h"
#include "nested_exception.h"
#include "serialization_data_source.h" // IWYU pragma: keep
#include "serialization_message.h" // IWYU pragma: keep
#include "scoped_stack_push.h"
#include "message_counters.h"

namespace docwire
{

using mailio::mime;
using mailio::codec;

namespace
{

struct context
{
	const message_callbacks& emit_message;
};

} // anonymous namespace

template<>
struct pimpl_impl<EMLParser> : pimpl_impl_base
{
	std::stack<context> m_context_stack;

	template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

	template <typename T>
	continuation emit_message_back(T&& object) const
	{
		return m_context_stack.top().emit_message.back(std::forward<T>(object));
	}

	void convertToUtf8(const std::string& charset, std::string& text) const
	{
		log_scope(charset);
		try
		{
			charset_converter converter(charset, "UTF-8");
			text = converter.convert(text);
		}
		catch (std::exception&)
		{
			emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Cannot convert text to UTF-8", charset)));
		}
	}

	mime_type mime_type_from_mime_entity(const mime& mime_entity) const
	{
		log_scope();
		class mime_wrapper : public mailio::mime
		{
		public:
			mime_type get_mime_type() const
			{
				return mime_type { mime_type_as_str(content_type().type) + "/" + content_type().subtype };
			}
		};
		return static_cast<const mime_wrapper&>(mime_entity).get_mime_type();
	}

	void extractPlainText(const mime& mime_entity)
	{
		log_scope(std::string(mime_entity.name()), mime_entity.boundary(), mime_type_from_mime_entity(mime_entity));
		if (mime_entity.content_type().type == mime::media_type_t::TEXT && (mime_entity.content_disposition() != mime::content_disposition_t::ATTACHMENT || std::string(mime_entity.name()).empty()))
		{
			log_scope();
			std::string plain = mime_entity.content();

			plain.erase(std::remove(plain.begin(), plain.end(), '\r'), plain.end());

			bool skip_charset_decoding = false;
			if (!mime_entity.content_type().charset.empty())
			{
				log_scope();
				convertToUtf8(mime_entity.content_type().charset, plain);
				skip_charset_decoding = true;
			}
			if (mime_entity.content_type().subtype == "html" || mime_entity.content_type().subtype == "xhtml")
			{
				log_scope();
				try
				{
					emit_message_back(data_source {
						plain, mime_type{"text/html"}, confidence::very_high});
				}
				catch (std::exception&)
				{
					emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process HTML body part")));
				}
			}
			else
			{
				if (skip_charset_decoding)
				{
					log_scope();
					emit_message(document::Text{.text = plain});
				}
				else
				{
					log_scope();
					try
					{
						emit_message_back(data_source {
							plain, mime_type{"text/plain"}, confidence::very_high});
					}
					catch (std::exception&)
					{
						emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process text body part")));
					}
				}
			}
		}
		else if (mime_entity.content_type().type != mime::media_type_t::MULTIPART)
		{
			log_scope();
			std::string plain = mime_entity.content();
			std::string file_name = mime_entity.name();
			if (file_name.empty())
				return;

			log_entry(file_name);
			file_extension extension { std::filesystem::path{file_name} };
			auto result = emit_message(mail::Attachment{.name = file_name, .size = plain.length(), .extension = extension});
			if (result != continuation::skip)
			{
				try
				{
					emit_message_back(data_source { plain, mime_type_from_mime_entity(mime_entity), confidence::very_high});
				}
				catch (std::exception&)
				{
					emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process attachment", file_name)));
				}
			}
			emit_message(mail::CloseAttachment{});
		}

		if (mime_entity.content_type().subtype == "alternative")
		{
			log_scope();
			const mime* selected_part = nullptr;
			const auto& parts = mime_entity.parts();

			// 1. Try to find a non-empty HTML part
			for (const mime& m: parts)
			{
				if ((m.content_type().subtype == "html" || m.content_type().subtype == "xhtml") && !m.content().empty())
				{
					selected_part = &m;
					break;
				}
			}

			// 2. If no HTML, try to find a non-empty plain text part
			if (!selected_part)
			{
				for (const mime& m: parts)
				{
					if (m.content_type().subtype == "plain" && !m.content().empty())
					{
						selected_part = &m;
						break;
					}
				}
			}

			// 3. Fallback: use the first part if it exists (even if empty or other type)
			if (!selected_part && !parts.empty())
				selected_part = &parts[0];

			if (selected_part)
				extractPlainText(*selected_part);
		}
		else
		{
			log_scope(mime_entity.parts().size());
			for (const mime& m: mime_entity.parts())
				extractPlainText(m);
		}
	}
};

EMLParser::EMLParser() = default;

namespace
{

constexpr std::string_view boundary_delimiter = "--";

void normalize_line(std::string& line)
{
	log_scope(line);
	if (!line.empty() && line.back() == '\r')
		line.pop_back();
}

// Helper class to expose protected members of mailio::mime.
// We derive from mailio::mime to access the protected parse_header_line() method.
// This allows us to incrementally parse headers line-by-line and leverage mailio's
// existing logic for handling header folding and parameter extraction (like the boundary),
// without having to reimplement a full MIME header parser.
class HeaderParser : public mailio::mime
{
public:
	using mailio::mime::parse_header_line;
	using mailio::mime::content_type;
};

class BoundaryTracker
{
public:
	void process_line(const std::string& line, mailio::message& mime_entity)
	{
		if (is_boundary_line(line))
			handle_boundary_line(line, mime_entity);

		if (m_in_headers)
			handle_header_line(line);
	}

private:
	bool is_boundary_line(const std::string& line) const
	{
		return line.starts_with(boundary_delimiter);
	}

	void handle_boundary_line(const std::string& line, mailio::message& mime_entity)
	{
		for (size_t i = m_boundaries.size(); i > 0; --i)
		{
			const auto current_boundary_index = i - 1;
			const std::string& boundary = m_boundaries[current_boundary_index];

			if (boundary.empty())
				continue;
			
			const std::string boundary_prefix = std::string(boundary_delimiter) + boundary;
			const bool is_closing = line == boundary_prefix + std::string(boundary_delimiter);
			const bool is_new_part = !is_closing && (line == boundary_prefix);

			if (is_closing || is_new_part)
			{
				inject_missing_closers(current_boundary_index, mime_entity);
				m_boundaries.resize(is_closing ? current_boundary_index : current_boundary_index + 1);
				m_in_headers = is_new_part;
				if (is_new_part)
				{
					m_current_header_parser = HeaderParser{};
					m_current_header_accumulator.clear();
				}
				return;
			}
		}
	}

	void inject_missing_closers(size_t active_boundary_index, mailio::message& mime_entity)
	{
		for (size_t j = m_boundaries.size() - 1; j > active_boundary_index; --j)
		{
			std::string missing_closer = std::string(boundary_delimiter) + m_boundaries[j] + std::string(boundary_delimiter);
			mime_entity.parse_by_line(missing_closer);
		}
	}

	void handle_header_line(const std::string& line)
	{
		if (line.empty())
		{
			flush_header();
			m_in_headers = false;
			if (m_current_header_parser.content_type().type == mailio::mime::media_type_t::MULTIPART &&
				!m_current_header_parser.boundary().empty())
			{
				m_boundaries.push_back(m_current_header_parser.boundary());
			}
			m_current_header_parser = HeaderParser{};
			m_current_header_accumulator.clear();
		}
		else if (std::isspace(static_cast<unsigned char>(line[0])))
		{
			if (!m_current_header_accumulator.empty())
				m_current_header_accumulator += line;
		}
		else
		{
			flush_header();
			m_current_header_accumulator = line;
		}
	}

	void flush_header()
	{
		if (!m_current_header_accumulator.empty())
		{
			try
			{
				m_current_header_parser.parse_header_line(m_current_header_accumulator);
			}
			catch (const std::exception& e)
			{
				log_entry("Failed to parse header line", m_current_header_accumulator, e.what());
			}
			m_current_header_accumulator.clear();
		}
	}

	std::vector<std::string> m_boundaries;
	HeaderParser m_current_header_parser;
	std::string m_current_header_accumulator;
	bool m_in_headers = true;
};

mailio::message parse_message(const data_source& data, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
	log_scope(data);
	std::shared_ptr<std::istream> stream = data.istream();
	mailio::message mime_entity;
	mime_entity.line_policy(codec::line_len_policy_t::NONE);
	try {
		std::string line;
		BoundaryTracker tracker;

		while (std::getline(*stream, line))
		{
			normalize_line(line);
			log_entry(line);
			tracker.process_line(line, mime_entity);
			mime_entity.parse_by_line(line);
		}
		mime_entity.parse_by_line("\r\n");
	} catch (std::exception& e)
	{
		non_fatal_error_handler(std::current_exception());
	}
	return mime_entity;
}

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"message/rfc822"}
};

} // anonymous namespace

namespace
{

attributes::Metadata metaData(const mailio::message& mime_entity);

} // anonymous namespace

continuation EMLParser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log_scope(msg);
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	log_entry();
	try
	{
		message_counters counters;
		auto counting_callbacks = make_counted_message_callbacks(emit_message, counters);
		scoped::stack_push<context> context_guard{impl().m_context_stack, context{counting_callbacks}};
		mailio::message mime_entity = parse_message(data, [emit_message](std::exception_ptr e) { emit_message(std::move(e)); });
		emit_message(document::Document
			{
				.metadata = [&mime_entity]()
				{
					return metaData(mime_entity);
				}
			});
		impl().extractPlainText(mime_entity);
		if (counters.all_failed())
			throw make_error("No parts were successfully processed", errors::uninterpretable_data{});
		emit_message(document::CloseDocument{});
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("EML parsing failed"));
	}
	return continuation::proceed;
}

namespace
{

struct mailio_time { boost::local_time::local_date_time v; };

std::optional<std::chrono::sys_seconds> convert_impl(const mailio_time& ldt, convert::dest_type_tag<std::chrono::sys_seconds>) noexcept
{
	try
	{
		if (ldt.v.is_special())
			return std::nullopt;
		boost::posix_time::ptime pt = ldt.v.utc_time();
		static const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
		boost::posix_time::time_duration diff = pt - epoch;
		return std::chrono::sys_seconds(std::chrono::seconds(diff.total_seconds()));
	}
	catch (...)
	{
		return std::nullopt;
	}
}

attributes::Metadata metaData(const mailio::message& mime_entity)
{
	log_scope();
	attributes::Metadata metadata;
	metadata.author = mime_entity.from_to_string();
	metadata.creation_date = convert::try_to<std::chrono::sys_seconds>(mailio_time{mime_entity.date_time()});

	//in EML file format author is visible under key "From". And creation date is visible under key "Data".
	//So, should I repeat the same values or skip them?
	metadata.email_attrs = attributes::Email
	{
		.from = mime_entity.from_to_string(),
		.date = convert::to<std::chrono::sys_seconds>(mailio_time{mime_entity.date_time()})
	};

	std::string to = mime_entity.recipients_to_string();
	if (!to.empty())
		metadata.email_attrs->to = to;
	std::string subject = mime_entity.subject();
	if (!subject.empty())
		metadata.email_attrs->subject = subject;
	std::string reply_to = mime_entity.reply_address_to_string();
	if (!reply_to.empty())
		metadata.email_attrs->reply_to = reply_to;
	std::string sender = mime_entity.sender_to_string();
	if (!sender.empty())
		metadata.email_attrs->sender = sender;
	return metadata;
}

} // anonymous namespace

} // namespace docwire
