/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#include "xml_parser.h"

#include "data_source.h"
#include "document_elements.h"
#include "log_entry.h"
#include "log_scope.h"
#include "make_error.h"
#include "serialization_message.h" // IWYU pragma: keep
#include "xml_children.h"

namespace docwire
{

namespace
{
template<safety_policy safety_level>
void parseXmlData(const message_callbacks& emit_message, xml::children_view<safety_level> view)
{
	log::scope _{};
	for (auto node: view)
	{
		std::string_view tag_name = node.name();
		std::string_view full_tag_name = node.full_name();
		if (tag_name == "#text")
		{
			std::string_view text = node.content();
			if (!text.empty())
				emit_message(document::text{ std::string{text} });
		}
		else if (tag_name != "style" && full_tag_name != "o:DocumentProperties" &&
			full_tag_name != "o:CustomDocumentProperties" && full_tag_name != "w:binData")
		{
			if (full_tag_name == "w:p")
				emit_message(document::paragraph{});
			parseXmlData(emit_message, children(node));
			if (full_tag_name == "w:p")
				emit_message(document::close_paragraph{});
			else if (full_tag_name == "w:tab")
				emit_message(document::text{"\t"});
		}
	}
}

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/xml"},
	mime_type{"text/xml"}
};

} // anonymous namespace

template <safety_policy safety_level>
continuation xml_parser<safety_level>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log::scope _{ "msg"_v = msg };

	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	log_entry();
	try
	{
		emit_message(document::document{});
		xml::reader<safety_level> reader(data.string_view()); // Correctly uses the non-owning constructor
		parseXmlData(emit_message, children(reader));
		emit_message(document::close_document{});
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("XML parsing failed"));
	}
	return continuation::proceed;
}

template class DOCWIRE_XML_EXPORT xml_parser<strict>;
template class DOCWIRE_XML_EXPORT xml_parser<relaxed>;

} // namespace docwire
