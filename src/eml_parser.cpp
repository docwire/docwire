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
#include <string>

#include "eml_parser.h"

#include "error_tags.h"
#include "htmlcxx/html/CharsetConverter.h"
#include "importer.h"
#include <iostream>
#include "log.h"
#include "plain_text_writer.h"
#include <mailio/message.hpp>
#include "throw_if.h"

namespace docwire
{

using mailio::mime;
using mailio::message;
using mailio::codec;

namespace
{
	std::mutex charset_converter_mutex;	
} // anonymous namespace

struct EMLParser::Implementation
{
  EMLParser* m_owner;

	Implementation(EMLParser* owner)
    : m_owner(owner)
  {}

	void convertToUtf8(const std::string& charset, std::string& text)
	{
		try
		{
			std::lock_guard<std::mutex> charset_converter_mutex_lock(charset_converter_mutex);
			htmlcxx::CharsetConverter converter(charset, "UTF-8");
			text = converter.convert(text);
		}
		catch (htmlcxx::CharsetConverter::Exception& ex)
		{
			m_owner->sendTag(errors::make_nested_ptr(ex, make_error("Cannot convert text to UTF-8", charset)));
		}
	}

	void extractPlainText(const mime& mime_entity)
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
				m_owner->sendTag(data_source{plain, file_extension{".html"}});
			}
			else
			{
				if (skip_charset_decoding)
				{
					docwire_log(debug) << "Charset is specified and decoding is skipped";
					m_owner->sendTag(tag::Text{.text = plain});
				}
				else
				{
					docwire_log(debug) << "Charset is not specified";
					m_owner->sendTag(data_source{plain, file_extension{".txt"}});
				}
			}
			m_owner->sendTag(tag::Text{.text = "\n\n"});
			return;
		}
		else if (mime_entity.content_type().type != mime::media_type_t::MULTIPART)
		{
			docwire_log(debug) << "It is not a multipart message. It's attachment probably.";
			std::string plain = mime_entity.content();
			std::string file_name = mime_entity.name();
			docwire_log(debug) << "File name: " << file_name;
			file_extension extension { std::filesystem::path{file_name} };
			auto info = m_owner->sendTag(
				tag::Attachment{.name = file_name, .size = plain.length(), .extension = extension});
			if(!info.skip)
			{
				m_owner->sendTag(data_source{plain, extension});
			}
			m_owner->sendTag(tag::CloseAttachment{});
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

EMLParser::EMLParser()
	: impl(new Implementation(this))
{
}

EMLParser::~EMLParser() = default;

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

} // anonymous namespace

bool EMLParser::understands(const data_source& data) const
{
	docwire_log_func();
	message mime_entity = parse_message(data, [](std::exception_ptr) {});
	std::string from = mime_entity.from_to_string();
	bool has_from = !from.empty();
	bool has_date_time = !mime_entity.date_time().is_not_a_date_time();
	docwire_log_vars(from, has_from, has_date_time);
	return has_from && has_date_time;
}

namespace
{

attributes::Metadata metaData(const message& mime_entity);

} // anonymous namespace

void
EMLParser::parse(const data_source& data) const
{
	docwire_log_func();
	docwire_log(debug) << "Using EML parser.";
	throw_if (!understands(data), errors::uninterpretable_data{});
	message mime_entity = parse_message(data, [this](std::exception_ptr e) { sendTag(e); });
	sendTag(tag::Document
		{
			.metadata = [&mime_entity]()
			{
				return metaData(mime_entity);
			}
		});
	impl->extractPlainText(mime_entity);
	sendTag(tag::CloseDocument{});
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
