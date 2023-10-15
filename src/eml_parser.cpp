/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include <algorithm>
#include <string>

#include "eml_parser.h"

#include "attachment.h"
#include "exception.h"
#include "htmlcxx/html/CharsetConverter.h"
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
	const std::shared_ptr<ParserManager> m_parser_manager;

	Implementation(const std::shared_ptr<ParserManager> &inParserManager, EMLParser* owner)
    : m_parser_manager(inParserManager),
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
		auto parser_builder = m_parser_manager->findParserByExtension(type);
		if (parser_builder)
		{
			auto parser = (*parser_builder)->withParserManager(m_parser_manager)
							.withParameters(m_owner->m_parameters)
							.build(text.c_str(), text.length());
			parser->addOnNewNodeCallback(callback);
			parser->parse();
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
					if (m_parser_manager)
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
						if (m_parser_manager)
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

			if (m_parser_manager)
			{
			std::string file_name = mime_entity.name();
			docwire_log(debug) << "File name: " << file_name;
			std::string extension = file_name.substr(file_name.find_last_of(".") + 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			auto info = m_owner->sendTag(
				StandardTag::TAG_ATTACHMENT, "", {{"name", file_name}, {"size", plain.length()}, {"extension", extension}});
			if(!info.skip)
			{
				auto parser_builder = m_parser_manager->findParserByExtension(file_name);
				if (parser_builder)
				{
					auto parser = (*parser_builder)->withParserManager(m_parser_manager)
						.withOnNewNodeCallbacks({[this](Info &info){m_owner->sendTag(info.tag_name, info.plain_text, info.attributes);}})
						.build(plain.c_str(), plain.length());
						parser->parse();
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

EMLParser::EMLParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager)
  : Parser(inParserManager)
{
	impl = NULL;
	try
	{
		impl = new Implementation(inParserManager, this);
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

EMLParser::EMLParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager)
  : Parser(inParserManager)
{
	impl = NULL;
	try
	{
		impl = new Implementation(inParserManager, this);
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

message parse_message(std::istream& stream)
{
	message mime_entity;
	mime_entity.line_policy(codec::line_len_policy_t::RECOMMENDED, codec::line_len_policy_t::NONE);
	try {
		std::string line;
		while (getline(stream, line))
		{
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
	if (!impl->m_data_stream->good())
		throw Exception("Error opening file " + impl->m_file_name);
	message mime_entity = parse_message(*impl->m_data_stream);
	return (!mime_entity.from_to_string().empty()) && (!mime_entity.date_time().is_not_a_date_time());
}

std::string EMLParser::plainText(const FormattingStyle& formatting) const
{
	std::string text;
	if (!isEML())
		throw Exception("Specified file is not valid EML file");
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
