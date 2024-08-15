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

#include <functional>
#include "tags.h"
#include "xml_stream.h"

namespace docwire
{

bool XMLParser::understands(const data_source& data) const
{
	std::string signature = data.string(length_limit{5});
	return (signature == "<?xml");
}

namespace
{

void parseXmlData(std::function<void(const Tag&)> send_tag, XmlStream& xml_stream)
{
	while (xml_stream)
	{
		std::string tag_name = xml_stream.name();
		if (tag_name == "#text")
		{
			char* content = xml_stream.content();
			if (content != NULL)
			{
				std::string text = content;
				send_tag(tag::Text{ text });
			}
		}
		else if (tag_name != "style")
		{
			xml_stream.levelDown();
			if (xml_stream)
				parseXmlData(send_tag, xml_stream);
			xml_stream.levelUp();
		}
		xml_stream.next();
	}
}

} // anonymous namespace

void XMLParser::parse(const data_source& data) const
{
	sendTag(tag::Document{});
	std::string xml_content = data.string();
	XmlStream xml_stream(xml_content);
	parseXmlData([this](const Tag& tag) { this->sendTag(tag); }, xml_stream);
	sendTag(tag::CloseDocument{});
}

} // namespace docwire
