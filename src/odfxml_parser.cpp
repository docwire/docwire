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

#include "odfxml_parser.h"

#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "data_source.h"
#include "document_elements.h"
#include "make_error.h"
#include "serialization_enum.h" // IWYU pragma: keep
#include "serialization_message.h" // IWYU pragma: keep

namespace docwire
{

namespace
{

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.oasis.opendocument.text-flat-xml"},
	mime_type{"application/vnd.oasis.opendocument.spreadsheet-flat-xml"},
	mime_type{"application/vnd.oasis.opendocument.presentation-flat-xml"},
	mime_type{"application/vnd.oasis.opendocument.graphics-flat-xml"}
};

} // anonymous namespace
	
template <safety_policy safety_level, typename auditor_type>
struct pimpl_impl<odfxml_parser<safety_level, auditor_type>>
    : with_pimpl_owner<odfxml_parser<safety_level, auditor_type>>
{
	using owner_type = odfxml_parser<safety_level, auditor_type>;
	using with_pimpl_owner<owner_type>::owner;

	audit::auditor_ref<auditor_type> auditor() { return owner().auditor(); }
	audit::auditor_ref<auditor_type> auditor() const { return owner().auditor(); }

	void parse(const data_source& data, xml_parse_mode mode, const message_callbacks& emit_message);
	attributes::metadata extract_metadata(std::string_view xml_content) const;
	pimpl_impl(owner_type& owner)
		: with_pimpl_owner<owner_type>{owner}
	{}

		void onODFBody(xml::node_ref<safety_level>& xml_node, xml_parse_mode mode,
							  const zip_reader* zipfile, std::string& text,
							  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			audit::scope audit_scope{auditor(), audit::no_context};
			// warning TODO: Unfortunately, in common_xml_document_parser we are not checking full names for xml tags.\
			Thats a problem, since we can have table:body, office:body etc. What if more xml tags are not handled correctly?
			if (xml_node.full_name() != "office:body")
			{
				audit::entry(auditor(), audit::no_context);
				return;
			}
			//we are inside body, we can disable adding text nodes
			owner().disableText(false);
		}

		void onODFObject(xml::node_ref<safety_level>& xml_node, xml_parse_mode mode,
								zip_reader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			audit::scope audit_scope{auditor(), audit::no_context};
			owner().disableText(true);
			text += owner().parseXmlChildren(xml_node, mode, zipfile);
			owner().disableText(false);
		}

		void onODFBinaryData(xml::node_ref<safety_level>& xml_node, xml_parse_mode mode,
									const zip_reader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			audit::scope audit_scope{auditor(), audit::no_context};
			children_processed = true;
		}
};

template <safety_policy safety_level, typename auditor_type>
odfxml_parser<safety_level, auditor_type>::odfxml_parser(audit::auditor_ref<auditor_type> auditor)
	: common_xml_document_parser<safety_level, auditor_type>(auditor)
{
	registerODFOOXMLCommandHandler("body", [&impl=impl()](xml::node_ref<safety_level>& xml_node, xml_parse_mode mode, zip_reader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFBody(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("object", [&impl=impl()](xml::node_ref<safety_level>& xml_node, xml_parse_mode mode, zip_reader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFObject(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("binary-data", [&impl=impl()](xml::node_ref<safety_level>& xml_node, xml_parse_mode mode, zip_reader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onODFBinaryData(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
}

template <safety_policy safety_level, typename auditor_type>
void pimpl_impl<odfxml_parser<safety_level, auditor_type>>::parse(const data_source& data, xml_parse_mode mode, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "mode"_v = mode; }};
	std::string_view xml_content = data.string_view();
	auto base_context_guard = owner().create_base_context_guard(emit_message);

	emit_message(document::document
		{
			.metadata = [this, xml_content]()
			{
				return extract_metadata(xml_content);
			}
		});

	//according to the ODF specification, we must skip blank nodes. Otherwise output from flat xml will be messed up.
	owner().set_blanks(xml::reader_blanks::ignore);
	//in the beginning of xml stream, there are some options which we do not want to parse
	owner().disableText(true);
	try
	{
		std::string text;
		owner().extractText(xml_content, mode, nullptr, text);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Extracting text failed"));
	}
	emit_message(document::close_document{});
}

template <safety_policy safety_level, typename auditor_type>
attributes::metadata pimpl_impl<odfxml_parser<safety_level, auditor_type>>::extract_metadata(std::string_view xml_content) const
{
	audit::scope audit_scope{auditor(), audit::no_context};
	attributes::metadata metadata;

	owner().parseODFMetadata(xml_content, metadata); // Call owner's parseODFMetadata
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

template <safety_policy safety_level, typename auditor_type>
continuation odfxml_parser<safety_level, auditor_type>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};

	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted(); // General check on data_source

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
	{
		return emit_message(std::move(msg));
	}

	audit::entry(auditor(), audit::no_context);
	try
	{
		impl().parse(data, xml_parse_mode::PARSE_XML, emit_message);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("ODF Flat XML parsing failed"));
	}
	return continuation::proceed;
}

template class DOCWIRE_ODF_OOXML_EXPORT odfxml_parser<strict, audit::null_auditor>;
template class DOCWIRE_ODF_OOXML_EXPORT odfxml_parser<relaxed, audit::null_auditor>;
template class DOCWIRE_ODF_OOXML_EXPORT odfxml_parser<docwire::safety_policy::strict, docwire::audit::json_stream_auditor>;
template class DOCWIRE_ODF_OOXML_EXPORT odfxml_parser<docwire::safety_policy::relaxed, docwire::audit::json_stream_auditor>;

} // namespace docwire
