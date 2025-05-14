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

template<>
struct pimpl_impl<ODFXMLParser> : with_pimpl_owner<ODFXMLParser>
{
	pimpl_impl(ODFXMLParser& owner) : with_pimpl_owner{owner} {}

		void onODFBody(XmlStream& xml_stream, XmlParseMode mode,
							  const ZipReader* zipfile, std::string& text,
							  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			// warning TODO: Unfortunately, in CommonXMLDocumentParser we are not checking full names for xml tags.\
			Thats a problem, since we can have table:body, office:body etc. What if more xml tags are not handled correctly?
			if (xml_stream.fullName() != "office:body")
				return;
			docwire_log(debug) << "ODF_BODY Command";
			//we are inside body, we can disable adding text nodes
			owner().disableText(false);
		}

		void onODFObject(XmlStream& xml_stream, XmlParseMode mode,
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_OBJECT Command";
			xml_stream.levelDown();
			owner().disableText(true);
			text += owner().parseXmlData(xml_stream, mode, zipfile);
			owner().disableText(false);
			xml_stream.levelUp();
		}

		void onODFBinaryData(XmlStream& xml_stream, XmlParseMode mode,
									const ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_BINARY_DATA Command";
			children_processed = true;
		}
};

ODFXMLParser::ODFXMLParser()
{
	registerODFOOXMLCommandHandler("body", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFBody(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("object", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFObject(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("binary-data", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFBinaryData(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
}

void ODFXMLParser::parse(const data_source& data, XmlParseMode mode, const emission_callbacks& emit_tag)
{
	std::string xml_content = data.string();

	CommonXMLDocumentParser::scoped_context_stack_push base_context_guard{*this, emit_tag};

	emit_tag(tag::Document
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
	emit_tag(tag::CloseDocument{});
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

void ODFXMLParser::parse(const data_source& data, const emission_callbacks& emit_tag)
{
	docwire_log(debug) << "Using ODFXML parser.";
	parse(data, XmlParseMode::PARSE_XML, emit_tag);
}

} // namespace docwire
