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

#include <libxml/xmlreader.h>
#include "log.h"
#include "make_error.h"
#include "xml_stream.h"

namespace docwire
{

class ODFXMLParser::CommandHandlersSet
{
	public:
		static void onODFBody(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
							  const ZipReader* zipfile, std::string& text,
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
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_OBJECT Command";
			xml_stream.levelDown();
			parser.disableText(true);
			text += parser.parseXmlData(xml_stream, mode, zipfile);
			parser.disableText(false);
			xml_stream.levelUp();
		}

		static void onODFBinaryData(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									const ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_BINARY_DATA Command";
			children_processed = true;
		}
};

ODFXMLParser::ODFXMLParser()
{
		registerODFOOXMLCommandHandler("body", &CommandHandlersSet::onODFBody);
		registerODFOOXMLCommandHandler("object", &CommandHandlersSet::onODFObject);
		registerODFOOXMLCommandHandler("binary-data", &CommandHandlersSet::onODFBinaryData);
}

void ODFXMLParser::parse(const data_source& data, XmlParseMode mode)
{
	std::string xml_content = data.string();

	trySendTag(tag::Document
		{
			.metadata = [this, &xml_content]()
			{
				return metaData(xml_content);
			}
		});

	//according to the ODF specification, we must skip blank nodes. Otherwise output from flat xml will be messed up.
	setXmlOptions(XML_PARSE_NOBLANKS);
	//in the beggining of xml stream, there are some options which we do not want to parse
	disableText(true);
	try
	{
		std::string text;
		extractText(xml_content, mode, NULL, text);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Extracting text failed"));
	}
	trySendTag(tag::CloseDocument{});
}

attributes::Metadata ODFXMLParser::metaData(const std::string& xml_content) const
{
	docwire_log(debug) << "Extracting metadata.";
	attributes::Metadata metadata;

	parseODFMetadata(xml_content, metadata);
	if (!metadata.page_count)
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
			metadata.page_count = page_count;
		}
	}
	return metadata;
}

void ODFXMLParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using ODFXML parser.";
	parse(data, XmlParseMode::PARSE_XML);
}

} // namespace docwire
