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

#include "xml_parser.h"

#include "tags.h"
#include "xml_stream.h"

namespace docwire
{

namespace
{

void parseXmlData(const emission_callbacks& emit_tag, XmlStream& xml_stream)
{
	while (xml_stream)
	{
		std::string tag_name = xml_stream.name();
		std::string full_tag_name = xml_stream.fullName();
		if (tag_name == "#text")
		{
			char* content = xml_stream.content();
			if (content != NULL)
			{
				std::string text = content;
				emit_tag(tag::Text{ text });
			}
		}
		else if (tag_name != "style" && full_tag_name != "o:DocumentProperties" &&
			full_tag_name != "o:CustomDocumentProperties" && full_tag_name != "w:binData")
		{
			if (full_tag_name == "w:p")
				emit_tag(tag::Paragraph{});
			xml_stream.levelDown();
			if (xml_stream)
				parseXmlData(emit_tag, xml_stream);
			xml_stream.levelUp();
			if (full_tag_name == "w:p")
				emit_tag(tag::CloseParagraph{});
			else if (full_tag_name == "w:tab")
				emit_tag(tag::Text{"\t"});
		}
		xml_stream.next();
	}
}

} // anonymous namespace

void XMLParser::parse(const data_source& data, const emission_callbacks& emit_tag)
{
	emit_tag(tag::Document{});
	std::string xml_content = data.string();
	XmlStream xml_stream(xml_content);
	parseXmlData(emit_tag, xml_stream);
	emit_tag(tag::CloseDocument{});
}

} // namespace docwire
