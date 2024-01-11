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

#include "odfxml_parser.h"

#include "exception.h"
#include <fstream>
#include <iostream>
#include <boost/signals2.hpp>
#include <libxml/xmlreader.h>
#include "log.h"
#include "metadata.h"
#include "misc.h"
#include "xml_stream.h"

namespace docwire
{

class ODFXMLParser::CommandHandlersSet
{
	public:
		static void onODFBody(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
							  const FormattingStyle& options, const ZipReader* zipfile, std::string& text,
							  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			// warning TODO: Unfortunately, in CommonXMLDocumentParser we are not checking full names for xml tags.\
			Thats a problem, since we can have table:body, office:body etc. What if more xml tags are not handled correctly?
			if (xml_stream.fullName() != "office:body")
				return;
			docwire_log(debug) << "ODF_BODY Command";
			//we are inside body, we can disable adding text nodes
			parser.disableText(false);
		}

		static void onODFObject(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								const FormattingStyle& options, const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_OBJECT Command";
			xml_stream.levelDown();
			parser.disableText(true);
			text += parser.parseXmlData(xml_stream, mode, options, zipfile);
			parser.disableText(false);
			xml_stream.levelUp();
		}

		static void onODFBinaryData(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									const FormattingStyle& options, const ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_BINARY_DATA Command";
			children_processed = true;
		}
};

struct ODFXMLParser::ExtendedImplementation
{
	const char* m_buffer;
	size_t m_buffer_size;
	std::string m_file_name;
	ODFXMLParser* m_interf;
	boost::signals2::signal<void(Info &info)> m_on_new_node_signal;
};

ODFXMLParser::ODFXMLParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager)
  : Parser(inParserManager)
{
	extended_impl = NULL;
	try
	{
		extended_impl = new ExtendedImplementation();
		extended_impl->m_file_name = file_name;
		extended_impl->m_buffer = NULL;
		extended_impl->m_buffer_size = 0;
		extended_impl->m_interf = this;
		registerODFOOXMLCommandHandler("body", &CommandHandlersSet::onODFBody);
		registerODFOOXMLCommandHandler("object", &CommandHandlersSet::onODFObject);
		registerODFOOXMLCommandHandler("binary-data", &CommandHandlersSet::onODFBinaryData);
	}
	catch (std::bad_alloc& ba)
	{
		if (extended_impl)
			delete extended_impl;
		cleanUp();
		throw;
	}
}

ODFXMLParser::ODFXMLParser(const char *buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager)
  : Parser(inParserManager)
{
	extended_impl = NULL;
	try
	{
		extended_impl = new ExtendedImplementation();
		extended_impl->m_file_name = "Memory buffer";
		extended_impl->m_buffer = buffer;
		extended_impl->m_buffer_size = size;
		extended_impl->m_interf = this;
		registerODFOOXMLCommandHandler("body", &CommandHandlersSet::onODFBody);
		registerODFOOXMLCommandHandler("object", &CommandHandlersSet::onODFObject);
		registerODFOOXMLCommandHandler("binary-data", &CommandHandlersSet::onODFBinaryData);
	}
	catch (std::bad_alloc& ba)
	{
		if (extended_impl)
			delete extended_impl;
		cleanUp();
		throw;
	}
}

ODFXMLParser::~ODFXMLParser()
{
	if (extended_impl)
		delete extended_impl;
}

bool ODFXMLParser::isODFXML()
{
	std::string xml_content;
	if (extended_impl->m_buffer_size > 0)
		xml_content = std::string(extended_impl->m_buffer, extended_impl->m_buffer_size);
	else
	{
		std::ifstream file(extended_impl->m_file_name.c_str(), std::ios_base::in|std::ios_base::binary);
		if (!file.is_open())
			throw Exception("Error opening file: " + extended_impl->m_file_name);
		xml_content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	}
	if (xml_content.find("office:document") == std::string::npos)
		return false;
	return true;
}

std::string ODFXMLParser::plainText(XmlParseMode mode, FormattingStyle& formatting_style) const
{
	std::string text;
	std::string xml_content;
	if (extended_impl->m_buffer_size > 0)
		xml_content = std::string(extended_impl->m_buffer, extended_impl->m_buffer_size);
	else
	{
		std::ifstream file(extended_impl->m_file_name.c_str(), std::ios_base::in|std::ios_base::binary);
		if (!file.is_open())
			throw Exception("Error opening file: " + extended_impl->m_file_name);
		xml_content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	}

	//according to the ODF specification, we must skip blank nodes. Otherwise output from flat xml will be messed up.
	setXmlOptions(XML_PARSE_NOBLANKS);
	//in the beggining of xml stream, there are some options which we do not want to parse
	disableText(true);
	try
	{
		extractText(xml_content, mode, formatting_style, NULL, text);
	}
	catch (Exception& ex)
	{
		ex.appendError("Error parsing Flat XML file");
		throw;
	}
	return text;
}

Metadata ODFXMLParser::metaData() const
{
	docwire_log(debug) << "Extracting metadata.";
	Metadata metadata;

	std::string xml_content;
	if (extended_impl->m_buffer_size > 0)
		xml_content = std::string(extended_impl->m_buffer, extended_impl->m_buffer_size);
	else
	{
		std::ifstream file(extended_impl->m_file_name.c_str(), std::ios_base::in|std::ios_base::binary);
		if (!file.is_open())
			throw Exception("Error opening file: " + extended_impl->m_file_name);
		xml_content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	}
	try
	{
		parseODFMetadata(xml_content, metadata);
	}
	catch (Exception& ex)
	{
		ex.appendError("Error parsing metadata in Flat XML file");
		throw;
	}
	if (metadata.pageCount() == -1)
	{
		// If we are processing ODP use slide count as page count
		// If we are processing ODG extract page count the same way
		if (xml_content.find("<office:presentation") != std::string::npos ||
			xml_content.find("<office:drawing") != std::string::npos)
		{
			int page_count = 0;
			std::string page_str = "<draw:page ";
			for (size_t pos = xml_content.find(page_str); pos != std::string::npos;
					pos = xml_content.find(page_str, pos + page_str.length()))
				page_count++;
			metadata.setPageCount(page_count);
		}
	}
	return metadata;
}

Parser&
ODFXMLParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
	return *this;
}

void
ODFXMLParser::parse() const
{
	docwire_log(debug) << "Using ODFXML parser.";
	auto formatting_style = getFormattingStyle();
  plainText(XmlParseMode::PARSE_XML, formatting_style);
	Metadata meta_data = metaData();
  Info info(StandardTag::TAG_METADATA, "", meta_data.getFieldsAsAny());
  extended_impl->m_on_new_node_signal(info);
}

Parser&
ODFXMLParser::addOnNewNodeCallback(NewNodeCallback callback)
{
  extended_impl->m_on_new_node_signal.connect(callback);
  CommonXMLDocumentParser::addCallback(callback);
  return *this;
}

} // namespace docwire
