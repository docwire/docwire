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

#include "attachment.h"
#include "exception.h"
#include "htmlcxx/html/CharsetConverter.h"
#include "importer.h"
#include <iostream>
#include "log.h"
#include "metadata.h"
#include "plain_text_writer.h"
#include <pthread.h>
#include <mailio/message.hpp>

namespace docwire
{

using mailio::mime;
using mailio::message;
using mailio::codec;

pthread_mutex_t charset_converter_mutex = PTHREAD_MUTEX_INITIALIZER;

struct EMLParser::Implementation
{
  EMLParser* m_owner;
	bool m_error;
	std::string m_file_name;
	std::istream* m_data_stream;
	const Importer* m_importer;

	Implementation(const Importer* inImporter, EMLParser* owner)
    : m_importer(inImporter),
      m_owner(owner)
  {}

	void convertToUtf8(const std::string& charset, std::string& text)
	{
		try
		{
			pthread_mutex_lock(&charset_converter_mutex);
			htmlcxx::CharsetConverter converter(charset, "UTF-8");
			text = converter.convert(text);
			pthread_mutex_unlock(&charset_converter_mutex);
		}
		catch (htmlcxx::CharsetConverter::Exception& ex)
		{
			pthread_mutex_unlock(&charset_converter_mutex);
			docwire_log(warning) << "Warning: Cant convert text to UTF-8 from " + charset;
		}
	}

	std::string parseText(const std::string &text, const std::string &type)
	{
		std::string parsed_text;
		PlainTextWriter writer;
		std::stringstream stream;
		auto callback = [&writer, &stream](const Info &info){writer.write_to(info, stream);};
		auto parser_builder = m_importer->findParserByExtension(type);
		if (parser_builder)
		{
			auto parser = (*parser_builder)->withImporter(*m_importer)
							.withParameters(m_owner->m_parameters)
							.build(text.c_str(), text.length());
			parser->addOnNewNodeCallback(callback);
			parser->parse();
			delete *parser_builder;
		}
		return stream.str();
	}

	void extractPlainText(const mime& mime_entity, std::string& output, const FormattingStyle& formatting)
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
				try
				{
					if (m_importer)
					{
						plain = parseText(plain, "html");
					}
				}
				catch (Exception& ex)
				{
					docwire_log(warning) << "Warning: Error while parsing html content";
				}
			}
			else
			{
				if (!skip_charset_decoding)
				{
					docwire_log(debug) << "Charset is not specified";
					try
					{
						if (m_importer)
						{
							plain = parseText(plain, "txt");
						}
					}
					catch (Exception& ex)
					{
						docwire_log(warning) << "Warning: Error while parsing text content";
					}
				}
			}

			m_owner->sendTag(StandardTag::TAG_TEXT, plain + "\n\n");

			output += plain;
			output += "\n\n";
			return;
		}
		else if (mime_entity.content_type().type != mime::media_type_t::MULTIPART)
		{
			docwire_log(debug) << "It is not a multipart message. It's attachment probably.";
			std::string plain = mime_entity.content();

			if (m_importer)
			{
			std::string file_name = mime_entity.name();
			docwire_log(debug) << "File name: " << file_name;
			std::string extension = file_name.substr(file_name.find_last_of(".") + 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			auto info = m_owner->sendTag(
				StandardTag::TAG_ATTACHMENT, "", {{"name", file_name}, {"size", plain.length()}, {"extension", extension}});
			if(!info.skip)
			{
				auto parser_builder = m_importer->findParserByExtension(file_name);
				if (parser_builder)
				{
					auto parser = (*parser_builder)->withImporter(*m_importer)
						.withOnNewNodeCallbacks({[this](Info &info){m_owner->sendTag(info.tag_name, info.plain_text, info.attributes);}})
						.build(plain.c_str(), plain.length());
						parser->parse();
					delete *parser_builder;
				}
			}
			m_owner->sendTag(StandardTag::TAG_CLOSE_ATTACHMENT);
			}
		}
		if (mime_entity.content_type().subtype == "alternative")
		{
			docwire_log(debug) << "Alternative content subtype detected";
			bool html_found = false;
			for (const mime& m: mime_entity.parts())
				if (m.content_type().subtype == "html" || m.content_type().subtype == "xhtml")
				{
					extractPlainText(m, output, formatting);
					html_found = true;
				}
			if (!html_found && mime_entity.parts().size() > 0)
				extractPlainText(mime_entity.parts()[0], output, formatting);
		}
		else
		{
			docwire_log(debug) << "Multipart but not alternative";
			docwire_log(debug) << mime_entity.parts().size() << " mime parts found";
			for (const mime& m: mime_entity.parts())
				extractPlainText(m, output, formatting);
		}
	}
};

EMLParser::EMLParser(const std::string& file_name, const Importer* inImporter)
  : Parser(inImporter)
{
	impl = NULL;
	try
	{
		impl = new Implementation(inImporter, this);
		impl->m_data_stream = NULL;
		impl->m_data_stream = new std::ifstream(file_name.c_str());
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

EMLParser::EMLParser(const char* buffer, size_t size, const Importer* inImporter)
  : Parser(inImporter)
{
	impl = NULL;
	try
	{
		impl = new Implementation(inImporter, this);
		impl->m_data_stream = NULL;
		impl->m_data_stream = new std::stringstream(std::string(buffer, size));
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

EMLParser::~EMLParser()
{
	if (impl)
	{
		if (impl->m_data_stream)
			delete impl->m_data_stream;
		delete impl;
	}
}

namespace
{

void normalize_line(std::string& line)
{
	docwire_log_func_with_args(line);
	if (!line.empty() && line.back() == '\r')
		line.pop_back();
}

message parse_message(std::istream& stream)
{
	message mime_entity;
	mime_entity.line_policy(codec::line_len_policy_t::RECOMMENDED, codec::line_len_policy_t::NONE);
	try {
		std::string line;
		while (getline(stream, line))
		{
			normalize_line(line);
			docwire_log_var(line);
			mime_entity.parse_by_line(line);
		}
		mime_entity.parse_by_line("\r\n");
	} catch (std::exception& e)
	{
		docwire_log(error) << e.what();
	}
	return mime_entity;
}

} // anonymous namespace

bool EMLParser::isEML() const
{
	docwire_log_func();
	if (!impl->m_data_stream->good())
	{
		docwire_log(error) << "Error opening file " << impl->m_file_name;
		throw Exception("Error opening file " + impl->m_file_name);
	}
	message mime_entity = parse_message(*impl->m_data_stream);
	std::string from = mime_entity.from_to_string();
	bool has_from = !from.empty();
	bool has_date_time = !mime_entity.date_time().is_not_a_date_time();
	docwire_log_vars(from, has_from, has_date_time);
	return has_from && has_date_time;
}

std::string EMLParser::plainText(const FormattingStyle& formatting) const
{
	docwire_log_func();
	std::string text;
	if (!isEML())
	{
		docwire_log(error) << "Specified file is not valid EML file";
		throw Exception("Specified file is not valid EML file");
	}
	docwire_log(debug) << "stream_pos=" << impl->m_data_stream->tellg();
	impl->m_data_stream->clear();
	if (!impl->m_data_stream->seekg(0, std::ios_base::beg))
	{
		docwire_log(error) << "Stream seek operation failed";
		throw Exception("Stream seek operation failed");
	}
	message mime_entity = parse_message(*impl->m_data_stream);
	impl->extractPlainText(mime_entity, text, formatting);
	return text;
}

Metadata EMLParser::metaData()
{
	Metadata metadata;
	impl->m_data_stream->clear();
	if (!impl->m_data_stream->seekg(0, std::ios_base::beg))
	{
		docwire_log(error) << "Stream seek operation failed";
		throw Exception("Stream seek operation failed");
	}
	if (!isEML())
		throw Exception("Specified file is not valid EML file");
	impl->m_data_stream->clear();
	if (!impl->m_data_stream->seekg(0, std::ios_base::beg))
	{
		docwire_log(error) << "Stream seek operation failed";
		throw Exception("Stream seek operation failed");
	}
	message mime_entity = parse_message(*impl->m_data_stream);
	metadata.setAuthor(mime_entity.from_to_string());
	metadata.setCreationDate(to_tm(mime_entity.date_time()));

	//in EML file format author is visible under key "From". And creation date is visible under key "Data".
	//So, should I repeat the same values or skip them?
	metadata.addField("From", mime_entity.from_to_string());
	metadata.addField("Date", to_tm(mime_entity.date_time()));

	std::string to = mime_entity.recipients_to_string();
	if (!to.empty())
		metadata.addField("To", to);

	std::string subject = mime_entity.subject();
	if (!subject.empty())
		metadata.addField("Subject", subject);
	std::string reply_to = mime_entity.reply_address_to_string();
	if (!reply_to.empty())
		metadata.addField("Reply-To", reply_to);
	std::string sender = mime_entity.sender_to_string();
	if (!sender.empty())
		metadata.addField("Sender", sender);
	return metadata;
}

void
EMLParser::parse() const
{
	docwire_log(debug) << "Using EML parser.";
  plainText(getFormattingStyle());
}

} // namespace docwire
