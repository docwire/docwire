/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "xml_parser.h"

#include "audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "data_source.h"
#include "document_elements.h"
#include "make_error.h"
#include "serialization_message.h" // IWYU pragma: keep
#include "xml_children.h"

namespace df0
{

namespace
{
template<safety_policy safety_level, typename auditor_type>
void parseXmlData(audit::auditor_ref<auditor_type> auditor,
                  const message_callbacks& emit_message,
                  xml::children_view<safety_level, auditor_type> view)
{
	audit::scope audit_scope{auditor, audit::no_context};
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
			parseXmlData(auditor, emit_message, children(node, auditor));
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

template <safety_policy safety_level, typename auditor_type>
continuation xml_parser<safety_level, auditor_type>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};

	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	audit::entry(auditor(), audit::no_context);
	try
	{
		emit_message(document::document{});
		xml::reader<safety_level> reader(data.string_view()); // Correctly uses the non-owning constructor
		parseXmlData(auditor(), emit_message, children(reader, auditor()));
		emit_message(document::close_document{});
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("XML parsing failed"));
	}
	return continuation::proceed;
}

template class DF0_XML_EXPORT xml_parser<strict, audit::null_auditor>;
template class DF0_XML_EXPORT xml_parser<relaxed, audit::null_auditor>;
template class DF0_XML_EXPORT xml_parser<df0::safety_policy::strict, df0::audit::json_stream_auditor>;
template class DF0_XML_EXPORT xml_parser<df0::safety_policy::relaxed, df0::audit::json_stream_auditor>;

} // namespace df0
