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
#include <stack>
#include <string>

#include "eml_parser.h"

#include "charset_converter.h"
#include "data_source.h"
#include <iostream>
#include "log.h"
#include <mailio/message.hpp>
#include <mailio/mime.hpp>
#include "make_error.h"
#include "scoped_stack_push.h"
#include "tags.h"

namespace docwire
{

using mailio::mime;
using mailio::message;
using mailio::codec;

namespace
{

struct context
{
	const emission_callbacks& emit_tag;
};

} // anonymous namespace

template<>
struct pimpl_impl<EMLParser> : pimpl_impl_base
{
	std::stack<context> m_context_stack;

	continuation emit_tag(Tag&& tag) const
	{
		return m_context_stack.top().emit_tag(std::move(tag));
	}

	continuation emit_tag_back(data_source&& data) const
	{
		return m_context_stack.top().emit_tag.back(std::move(data));
	}

	void convertToUtf8(const std::string& charset, std::string& text) const
	{
		try
		{
			charset_converter converter(charset, "UTF-8");
			text = converter.convert(text);
		}
		catch (std::exception&)
		{
			emit_tag(errors::make_nested_ptr(std::current_exception(), make_error("Cannot convert text to UTF-8", charset)));
		}
	}

	mime_type mime_type_from_mime_entity(const mime& mime_entity) const
	{
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

	void extractPlainText(const mime& mime_entity) const
	{
		docwire_log(debug) << "Extracting plain text from mime entity";
		if (mime_entity.content_disposition() != mime::content_disposition_t::ATTACHMENT && mime_entity.content_type().type == mime::media_type_t::TEXT)
		{
			docwire_log(debug) << "Text content type detected with inline or none content disposition";
			std::string plain = mime_entity.content();
			plain.erase(std::remove(plain.begin(), plain.end(), '\r'), plain.end());

			bool skip_charset_decoding = false;
			if (!mime_entity.content_type().charset.empty())
			{
				docwire_log(debug) << "Charset is specified";
				convertToUtf8(mime_entity.content_type().charset, plain);
				skip_charset_decoding = true;
			}
			if (mime_entity.content_type().subtype == "html" || mime_entity.content_type().subtype == "xhtml")
			{
				docwire_log(debug) << "HTML content subtype detected";
				emit_tag_back(data_source {
					plain, mime_type{"text/html"}, confidence::very_high});
			}
			else
			{
				if (skip_charset_decoding)
				{
					docwire_log(debug) << "Charset is specified and decoding is skipped";
					emit_tag(tag::Text{.text = plain});
				}
				else
				{
					docwire_log(debug) << "Charset is not specified";
					emit_tag_back(data_source {
						plain, mime_type{"text/plain"}, confidence::very_high});
				}
			}
			emit_tag(tag::Text{.text = "\n\n"});
			return;
		}
		else if (mime_entity.content_type().type != mime::media_type_t::MULTIPART)
		{
			docwire_log(debug) << "It is not a multipart message. It's attachment probably.";
			std::string plain = mime_entity.content();
			std::string file_name = mime_entity.name();
			docwire_log(debug) << "File name: " << file_name;
			file_extension extension { std::filesystem::path{file_name} };
			auto result = emit_tag(
				tag::Attachment{.name = file_name, .size = plain.length(), .extension = extension});
			if (result != continuation::skip)
			{
				emit_tag_back(data_source {
					plain, mime_type_from_mime_entity(mime_entity), confidence::very_high});
			}
			emit_tag(tag::CloseAttachment{});
		}
		if (mime_entity.content_type().subtype == "alternative")
		{
			docwire_log(debug) << "Alternative content subtype detected";
			bool html_found = false;
			for (const mime& m: mime_entity.parts())
				if (m.content_type().subtype == "html" || m.content_type().subtype == "xhtml")
				{
					extractPlainText(m);
					html_found = true;
				}
			if (!html_found && mime_entity.parts().size() > 0)
				extractPlainText(mime_entity.parts()[0]);
		}
		else
		{
			docwire_log(debug) << "Multipart but not alternative";
			docwire_log(debug) << mime_entity.parts().size() << " mime parts found";
			for (const mime& m: mime_entity.parts())
				extractPlainText(m);
		}
	}
};

EMLParser::EMLParser() = default;

namespace
{

void normalize_line(std::string& line)
{
	docwire_log_func_with_args(line);
	if (!line.empty() && line.back() == '\r')
		line.pop_back();
}

message parse_message(const data_source& data, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
	std::shared_ptr<std::istream> stream = data.istream();
	message mime_entity;
	mime_entity.line_policy(codec::line_len_policy_t::NONE);
	try {
		std::string line;
		while (getline(*stream, line))
		{
			normalize_line(line);
			docwire_log_var(line);
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

attributes::Metadata metaData(const message& mime_entity);

} // anonymous namespace

continuation EMLParser::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
	docwire_log_func();
	if (!std::holds_alternative<data_source>(tag))
		return emit_tag(std::move(tag));

	auto& data = std::get<data_source>(tag);
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_tag(std::move(tag));

	docwire_log(debug) << "Using EML parser.";
	try
	{
		scoped::stack_push<context> context_guard{impl().m_context_stack, context{emit_tag}};
		message mime_entity = parse_message(data, [emit_tag](std::exception_ptr e) { emit_tag(e); });
		emit_tag(tag::Document
			{
				.metadata = [&mime_entity]()
				{
					return metaData(mime_entity);
				}
			});
		impl().extractPlainText(mime_entity);
		emit_tag(tag::CloseDocument{});
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("EML parsing failed"));
	}
	return continuation::proceed;
}

namespace
{

attributes::Metadata metaData(const message& mime_entity)
{
	attributes::Metadata metadata;
	metadata.author = mime_entity.from_to_string();
	metadata.creation_date = to_tm(mime_entity.date_time());

	//in EML file format author is visible under key "From". And creation date is visible under key "Data".
	//So, should I repeat the same values or skip them?
	metadata.email_attrs = attributes::Email
	{
		.from = mime_entity.from_to_string(),
		.date = to_tm(mime_entity.date_time())
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
