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

#include "html_parser.h"

#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <charsetdetect.h>
#include <lexbor/dom/dom.h>
#include <lexbor/dom/interface.h>
#include <lexbor/dom/interfaces/node.h>
#include <lexbor/html/encoding.h>
#include <lexbor/html/html.h>
#include <lexbor/html/interfaces/document.h>
#include <regex>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "charset_converter.h"
#include "log.h"
#include "make_error.h"
#include "misc.h"
#include <mutex>
#include <set>
#include "pimpl.h"
#include "scoped_stack_push.h"
#include "tags.h"
#include "throw_if.h"

namespace docwire
{

namespace
{

attributes::Styling html_node_styling(const lxb_dom_node_t* node)
{
    attributes::Styling styling;
    const lxb_dom_element_t* element = lxb_dom_interface_element(node);

    const lxb_char_t* class_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"class", 5, nullptr);
    if (class_attr) {
        std::string class_str((const char *)class_attr);
        boost::algorithm::split(styling.classes, class_str, boost::is_any_of(" "));
    }

    const lxb_char_t* id_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"id", 2, nullptr);
    if (id_attr) {
        styling.id = std::string((const char *)id_attr);
    }

    const lxb_char_t* style_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"style", 5, nullptr);
    if (style_attr) {
        styling.style = std::string((const char*)style_attr);
		boost::algorithm::trim(styling.style);
    }
    return styling;
}

void convert_to_utf8(std::string& text, const std::string& charset)
{
	if (boost::iequals(charset, "utf-8"))
		return;
	charset_converter converter(charset, "UTF-8");
	text = converter.convert(text);
}

std::optional<std::string> get_encoding_from_meta_tags(const std::string& html_content)
{
	std::unique_ptr<lxb_html_encoding_t, decltype([](lxb_html_encoding_t* h)
		{
			lxb_html_encoding_destroy(h, true);
		})>
		em
		{
			lxb_html_encoding_create()
		};
	throw_if (lxb_html_encoding_init(em.get()) != LXB_STATUS_OK);
	if (lxb_html_encoding_determine(em.get(), (const lxb_char_t*)html_content.data(), (lxb_char_t*)html_content.data() + html_content.length()) != LXB_STATUS_OK)
		return std::nullopt;
	lxb_html_encoding_entry_t* entry = lxb_html_encoding_meta_entry(em.get(), 0);
	if (entry == nullptr)
		return std::nullopt;
	std::string detected_encoding = std::string((const char*)entry->name, (size_t)(entry->end - entry->name));
	return detected_encoding;
}

std::optional<std::string> get_encoding_from_xml_declaration(const std::string& html_content)
{
	// if this is xhtml document, information about encoding may be stored between <?xml and ?>.
	// lexbor seems not to parse this fragment, so we should do this manually
	std::string charset;
	size_t initial_xml_start_pos = html_content.find("<?xml");
	size_t initial_xml_end_pos = html_content.find("?>");
	if (initial_xml_start_pos != std::string::npos && initial_xml_end_pos != std::string::npos
			&& initial_xml_end_pos > initial_xml_start_pos && initial_xml_end_pos < html_content.length())
	{
		std::string initial_xml = html_content.substr(initial_xml_start_pos, initial_xml_end_pos - initial_xml_start_pos);
		std::transform(initial_xml.begin(), initial_xml.end(), initial_xml.begin(), ::tolower);
		size_t encoding_pos = initial_xml.find("encoding");
		if (encoding_pos != std::string::npos)
		{
			encoding_pos += 7;
			while (encoding_pos < initial_xml.length() && initial_xml[encoding_pos] != '\"')
				++encoding_pos;
			++encoding_pos;
			while (encoding_pos < initial_xml.length() && initial_xml[encoding_pos] != '\"')
			{
				charset += initial_xml[encoding_pos];
				++encoding_pos;
			}
			return charset;
		}
	}
	return std::nullopt;
}

std::optional<std::string> guess_encoding(const std::string& text)
{
	std::unique_ptr<void, decltype([](csd_t h)
		{
			if (h != (csd_t)-1) csd_close(h);
		})>
		charset_detector
		{
			csd_open()
		};
	throw_if(charset_detector.get() == (csd_t)-1, "Could not create charset detector");
	csd_consider(charset_detector.get(), text.c_str(), static_cast<int>(text.length()));
	const char* res = csd_close(charset_detector.release());
	return res != nullptr ? std::optional<std::string>{res} : std::nullopt;
}

std::optional<std::string> determine_encoding(const std::string& html_content)
{
	std::optional<std::string> charset;
	try
	{
		charset = get_encoding_from_meta_tags(html_content);
	}
	catch (std::exception&)
	{
		std::throw_with_nested(make_error("Error determining encoding from meta tags"));
	}
	if (charset)
		return charset;
	charset = get_encoding_from_xml_declaration(html_content);
	if (charset)
		return charset;
	try
	{
		charset = guess_encoding(html_content);
	}
	catch (std::exception& e)
	{
		std::throw_with_nested(make_error("Error guessing encoding"));
	}
	return charset;
}

void ensure_html_utf8_encoding(std::string& html_content)
{
	std::optional<std::string> charset;
	try
	{
		charset = determine_encoding(html_content);
	}
	catch (std::exception&)
	{
		std::throw_with_nested(make_error("Error determining encoding"));
	}
	if (!charset)
	{
		docwire_log(debug) << "Could not detect encoding. Document is assumed to be encoded in UTF-8";
		charset = "UTF-8";
	}
	try
	{
		convert_to_utf8(html_content, *charset);
	}
	catch (std::exception&)
	{
		std::throw_with_nested(make_error("Cannot convert charset", *charset));
	}
	docwire_log(debug) << "After converting to utf8: [" << html_content << "]";
}

void move_child_node_to_parent(lxb_dom_node_t* node, lxb_dom_node_t* child_node)
{
	lxb_dom_node_remove_wo_events(child_node);
	lxb_dom_node_insert_before_wo_events(node, child_node);
}

void fix_dom_in_table_insertion_mode(lxb_dom_node_t* node, lxb_dom_node_t* child_node, lxb_tag_id_t child_tag_id)
{
	// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
	if (child_tag_id == LXB_TAG_CAPTION)
	{
		// "A start tag whose tag name is "caption""
		// "Insert an HTML element for the token, then switch the insertion mode to "in caption"."
		// Everything is OK.
	}
	else if (child_tag_id == LXB_TAG_TBODY || child_tag_id == LXB_TAG_TFOOT || child_tag_id == LXB_TAG_THEAD)
	{
		// "A start tag whose tag name is one of: "tbody", "tfoot", "thead""
		// "Insert an HTML element for the token, then switch the insertion mode to "in table body"."
		// Everything is OK.
	}
	else if (child_tag_id == LXB_TAG_TD || child_tag_id == LXB_TAG_TH || child_tag_id == LXB_TAG_TR)
	{
		// "A start tag whose tag name is one of: "td", "th", "tr""
		// "Insert an HTML element for a "tbody" start tag token with no attributes, then switch the insertion mode to "in table body"."
		// Everything is OK.
	}
	else if (child_tag_id == LXB_TAG_STYLE || child_tag_id == LXB_TAG_SCRIPT || child_tag_id == LXB_TAG_TEMPLATE)
	{
		// "A start tag whose tag name is one of: "style", "script", "template""
		// "Process the token using the rules for the "in head" insertion mode."
		// Move node to parent as a simple fix
		move_child_node_to_parent(node, child_node);
	}
	else
	{
		// "Anything else"
		// "Parse error. Enable foster parenting, process the token using the rules for the "in body" insertion mode, and then disable foster parenting."
		// Move node to parent as a simple fix
		move_child_node_to_parent(node, child_node);
	}
}

void fix_dom_in_table_body_insertion_mode(lxb_dom_node_t* node, lxb_dom_node_t* child_node, lxb_tag_id_t child_tag_id)
{
	// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intbody
	if (child_tag_id == LXB_TAG_TR)
	{
		// "A start tag whose tag name is "tr""
		// "Insert an HTML element for the token, then switch the insertion mode to "in row"."
		// Everything is OK.
	}
	else if (child_tag_id == LXB_TAG_TH || child_tag_id == LXB_TAG_TD)
	{
		// "A start tag whose tag name is one of: "th", "td""
		// "Insert an HTML element for a "tr" start tag token with no attributes, then switch the insertion mode to "in row"."
		// Everything is OK.
	}
	else
	{
		// "Anything else"
		// "Process the token using the rules for the "in table" insertion mode."
		fix_dom_in_table_insertion_mode(node, child_node, child_tag_id);
	}
}

void fix_dom_in_table_row_insertion_mode(lxb_dom_node_t* node, lxb_dom_node_t* child_node, lxb_tag_id_t child_tag_id)
{
	// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intr
	if (child_tag_id == LXB_TAG_TH || child_tag_id == LXB_TAG_TD)
	{
		// "A start tag whose tag name is one of: "th", "td""
		// "Insert an HTML element for the token, then switch the insertion mode to "in cell"."
		// Everything is OK.
	}
	else
	{
		// "Anything else"
		// "Process the token using the rules for the "in table" insertion mode."
		fix_dom_in_table_insertion_mode(node, child_node, child_tag_id);
	}
}

void fix_dom(lxb_dom_node_t* node)
{
	// First of all fix all child nodes as it may cause moving some nodes to the parent (this node)
	lxb_dom_node_t* child_node = lxb_dom_node_first_child(const_cast<lxb_dom_node_t*>(node));
    while (child_node != nullptr)
	{
        fix_dom(child_node);
        child_node = lxb_dom_node_next(child_node);
    }
	// Now all child nodes are correct but it does not mean that this node is correct
	if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
	{
		lxb_dom_element_t* element = lxb_dom_interface_element(node);
    	lxb_tag_id_t tag_id = lxb_dom_element_tag_id(element);
		lxb_dom_node_t* child_node = lxb_dom_node_first_child(node);
		while (child_node != nullptr)
		{
			lxb_dom_node_t* next_child_node = lxb_dom_node_next(child_node); // child_node may be moved
			if (child_node->type == LXB_DOM_NODE_TYPE_ELEMENT)
			{					
				const lxb_dom_element_t* child_element = lxb_dom_interface_element(child_node);
				lxb_tag_id_t child_tag_id = lxb_dom_element_tag_id(const_cast<lxb_dom_element_t*>(child_element));
				if (tag_id == LXB_TAG_TABLE)
					fix_dom_in_table_insertion_mode(node, child_node, child_tag_id);
				else if (tag_id == LXB_TAG_TBODY || tag_id == LXB_TAG_TFOOT || tag_id == LXB_TAG_THEAD)
					fix_dom_in_table_body_insertion_mode(node, child_node, child_tag_id);
				else if (tag_id == LXB_TAG_TR)
					fix_dom_in_table_row_insertion_mode(node, child_node, child_tag_id);
			}
			child_node = next_child_node;
		}
	}
}

struct context
{
	const emission_callbacks& emit_tag;
	bool turn_off_ul_enumeration = false;
	bool turn_off_ol_enumeration = false;
	bool head_parsed = false;
	bool in_metadata = false;
	bool in_head = false;
	bool in_title = false;
	bool in_script = false;
	bool in_style = false;
	std::string style_text;
	attributes::Metadata meta;
	std::string buffered_text;
	char last_char_in_inline_formatting_context = '\0';
};

} // unnamed namespace

template<>
struct pimpl_impl<HTMLParser> : pimpl_impl_base
{
	continuation emit_tag(Tag&& tag);
	void parse_css(const std::string & m_style_text);
	void parse_document(const std::string& html_content, const emission_callbacks& emit_tag);
	void process_node(const lxb_dom_node_t* node);
	void process_text(const lxb_dom_node_t* node);
	void process_tag(const lxb_dom_node_t* node, bool is_closing = false);
	bool m_skip_decoding = false;
	std::stack<context> m_context_stack;
};

continuation pimpl_impl<HTMLParser>::emit_tag(Tag&& tag)
{
	return m_context_stack.top().emit_tag(std::move(tag));
}

void pimpl_impl<HTMLParser>::parse_css(const std::string & m_style_text)
{
	// warning TODO: For now, we only need to know
	// about one attribute (list-style). This function only fixes the problem with automatic HTML files
	// produced by LibreOffice/OpenOffice. Those programs use <ol> and <ul>, but usually turn off
	// automatic list enumeration (list-style: none), and use own enumeration.
	// We can obtain output like this: "1. 1. first", "2. 2. second" etc.
	// We should look for something like: ol/ul/li { list-style: none }
	size_t search_position = 0;
	size_t found_position = 0;
	while ((found_position = m_style_text.find("list-style:", search_position)) != std::string::npos)
	{
		search_position = found_position + 11;
		//first, obtain style name:
		std::string style_name;
		for (size_t i = found_position + 11; i < m_style_text.length(); ++i, ++search_position)
		{
			if (m_style_text[i] == ';')
				break;
			if (!std::isspace(static_cast<unsigned char>(m_style_text[i])))
				style_name += m_style_text[i];
		}
		//now obtain which tags this rule (list-style) concerns.
		if (style_name == "none")
		{
			std::string match;
			int match_index = 0;
			for (size_t i = found_position; i > 0; --i)
			{
				if (m_style_text[i] == '{')
				{
					match_index = static_cast<int>(i) - 1;
					break;
				}
			}
			while (match_index >= 0)
			{
				if (std::isspace(static_cast<unsigned char>(m_style_text[match_index])))
				{
					if (match.length() > 0)
						break;
				}
				else
					match = m_style_text[match_index] + match;
				--match_index;
			}
			if (match == "li")
			{
				m_context_stack.top().turn_off_ol_enumeration = true;
				m_context_stack.top().turn_off_ul_enumeration = true;
			}
			else if (match == "ul")
				m_context_stack.top().turn_off_ul_enumeration = true;
			else if (match == "ol")
				m_context_stack.top().turn_off_ol_enumeration = true;
		}
	}
}

void pimpl_impl<HTMLParser>::parse_document(const std::string& html_content_arg, const emission_callbacks& emit_tag)
{
	std::string html_content = html_content_arg;
	if (!m_skip_decoding)
	{
		try
		{
			ensure_html_utf8_encoding(html_content);
		}
		catch (std::exception&)
		{
			emit_tag(make_nested_ptr(std::current_exception(), make_error("Error ensuring HTML is encoded in UTF-8")));
		}
	}

	scoped::stack_push<context> context_guard{m_context_stack, context{emit_tag}};

	lxb_html_document_t* document = lxb_html_document_create();
	throw_if(document == nullptr, "lxb_html_document_create failed");

	throw_if(lxb_html_document_parse(document, (const lxb_char_t *)html_content.data(), html_content.size()) != LXB_STATUS_OK,
		"Failed to parse HTML document");

	fix_dom(lxb_dom_interface_node(document));

	lxb_dom_node_t* head = lxb_dom_interface_node(lxb_html_document_head_element(document));
	parse_css((const char*)lxb_dom_node_text_content(head, nullptr));

	emit_tag(tag::Document
		{
			.metadata = [this, head]()
			{
				docwire_log(debug) << "Extracting metadata.";
				if (!m_context_stack.top().head_parsed)
				{
					m_context_stack.top().in_metadata = true;
					m_context_stack.top().in_head = true;
					process_node(head);
					m_context_stack.top().in_head = false;
					m_context_stack.top().in_metadata = false;
					m_context_stack.top().head_parsed = true;
				}
				return m_context_stack.top().meta;
			}
		});

	m_context_stack.top().in_head = true;
	process_node(head);
	m_context_stack.top().in_head = false;

	lxb_dom_node_t* body = lxb_dom_interface_node(lxb_html_document_body_element(document));
    process_node(body);

	lxb_html_document_destroy(document);
}

void pimpl_impl<HTMLParser>::process_node(const lxb_dom_node_t* node)
{
    if (node == nullptr)
        return;

    switch (node->type)
    {
        case LXB_DOM_NODE_TYPE_ELEMENT:
        {
            process_tag(node);
            break;
        }
        case LXB_DOM_NODE_TYPE_TEXT:
        {
			if (m_context_stack.top().in_metadata || m_context_stack.top().in_title || m_context_stack.top().in_script)
				break;
            process_text(node);
            break;
        }
        default:
            break;
    }

    lxb_dom_node_t* child_node = lxb_dom_node_first_child(const_cast<lxb_dom_node_t*>(node));
    while (child_node != nullptr)
	{
        process_node(child_node);
        child_node = lxb_dom_node_next(child_node);
    }
    if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
        process_tag(node, true);
}

void pimpl_impl<HTMLParser>::process_text(const lxb_dom_node_t* node)
{
	if (m_context_stack.top().in_head && !m_context_stack.top().in_style)
		return;
    const lxb_char_t* data = lxb_dom_node_text_content(const_cast<lxb_dom_node_t*>(node), nullptr);
    if (data == nullptr)
        return;
    std::string text((const char*)data);
	if (m_context_stack.top().in_style)
	{
		m_context_stack.top().style_text += text;
		return;
	}
	// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace#what_is_whitespace
	// Convert all whitespaces into spaces and reduce all adjacent spaces into a single space
	{
		static std::mutex whitespaces_regex_mutex;
		std::lock_guard<std::mutex> whitespaces_regex_mutex_lock(whitespaces_regex_mutex);
		static const std::regex whitespaces_regex(R"(\s+)");
		text = std::regex_replace(text, whitespaces_regex, " ");
	}
	docwire_log(debug) << "After converting and reducing whitespaces: [" << text << "]";
	bool last_char_was_space = isspace((unsigned char)m_context_stack.top().last_char_in_inline_formatting_context);
	docwire_log(debug) << "Last char in inline formatting context was whitespace: " << last_char_was_space;
	// Reduce whitespaces between text nodes (end of previous and beginning of current.
	// Remove whitespaces from beginning of inline formatting context.
	if (last_char_was_space || m_context_stack.top().last_char_in_inline_formatting_context == '\0')
	{
		boost::trim_left(text);
		docwire_log(debug) << "After reducing whitespaces between text nodes and removing whitespaces from begining of inline formatting context: [" << text << "]";
	}
	if (!text.empty())
	{
		m_context_stack.top().last_char_in_inline_formatting_context = text.back();
		// buffer text because whitespaces from end of inline formatting context should be removed.
		m_context_stack.top().buffered_text += text;
	}
}

void pimpl_impl<HTMLParser>::process_tag(const lxb_dom_node_t* node, bool is_closing)
{
    const lxb_dom_element_t* element = lxb_dom_interface_element(node);
    lxb_tag_id_t tag_id = lxb_dom_element_tag_id(const_cast<lxb_dom_element_t*>(element));
	if (!m_context_stack.top().buffered_text.empty())
	{
		// https://developer.mozilla.org/en-US/docs/Web/HTML/Block-level_elements
		// TODO: elements can have also block style in CSS
		static std::set<lxb_tag_id_t> block_level_elements = { LXB_TAG_ADDRESS, LXB_TAG_ARTICLE, LXB_TAG_ASIDE, LXB_TAG_BLOCKQUOTE, LXB_TAG_DETAILS, LXB_TAG_DIALOG, LXB_TAG_DD, LXB_TAG_DIV, LXB_TAG_DL,
			LXB_TAG_DT, LXB_TAG_FIELDSET, LXB_TAG_FIGCAPTION, LXB_TAG_FIGURE, LXB_TAG_FOOTER, LXB_TAG_FORM, LXB_TAG_H1, LXB_TAG_H2, LXB_TAG_H3, LXB_TAG_H4, LXB_TAG_H5, LXB_TAG_H6, LXB_TAG_HEADER, LXB_TAG_HGROUP, LXB_TAG_HR, LXB_TAG_LI,
			LXB_TAG_MAIN, LXB_TAG_NAV, LXB_TAG_OL, LXB_TAG_P, LXB_TAG_PRE, LXB_TAG_SECTION, LXB_TAG_TABLE, LXB_TAG_UL,
			LXB_TAG_HTML, LXB_TAG_BODY };
		// These elements are part of a table structure. We treat them like block-level elements for whitespace processing.
        static std::set<lxb_tag_id_t> table_elements = { LXB_TAG_TD, LXB_TAG_TR, LXB_TAG_TH, LXB_TAG_CAPTION, LXB_TAG_THEAD, LXB_TAG_TBODY, LXB_TAG_TFOOT };
		if (block_level_elements.contains(tag_id) || table_elements.contains(tag_id))
		{
			// Remove sequences of spaces at the end of an block-level element
			// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace
			boost::trim_right(m_context_stack.top().buffered_text);
			m_context_stack.top().last_char_in_inline_formatting_context = '\0'; // inline formatting context is now empty
		}
		if (!m_context_stack.top().buffered_text.empty())
		{
			emit_tag(tag::Text{.text = m_context_stack.top().buffered_text});
			m_context_stack.top().buffered_text = "";
		}
	}
    // All html elements that will emit tag::Paragraph (as we do not have H1 etc)
	std::set<lxb_tag_id_t> paragraph_elements = { LXB_TAG_H1, LXB_TAG_H2, LXB_TAG_H3, LXB_TAG_H4, LXB_TAG_H5, LXB_TAG_H6, LXB_TAG_P };	
    if (is_closing)
    {
		if (tag_id == LXB_TAG_STYLE)
		{
			m_context_stack.top().in_style = false;
			if (!m_context_stack.top().in_metadata)
				emit_tag(tag::Style{.css_text = m_context_stack.top().style_text});
			m_context_stack.top().style_text.clear();
		}
		else if (paragraph_elements.contains(tag_id))
		{
			emit_tag(tag::CloseParagraph{});
		}
		else if (tag_id == LXB_TAG_DIV)
		{
			emit_tag(tag::CloseSection{});
		}
		else if (tag_id == LXB_TAG_SPAN)
		{
			emit_tag(tag::CloseSpan{});
		}
		else if (tag_id == LXB_TAG_A)
		{
			emit_tag(tag::CloseLink{});
		}
		else if (tag_id == LXB_TAG_TABLE)
		{
			emit_tag(tag::CloseTable{});
		}
		else if (tag_id == LXB_TAG_CAPTION)
		{
			emit_tag(tag::CloseCaption{});
		}
		else if (tag_id == LXB_TAG_TR)
		{
			emit_tag(tag::CloseTableRow{});
		}
		else if (tag_id == LXB_TAG_TD || tag_id == LXB_TAG_TH)
		{
			emit_tag(tag::CloseTableCell{});
		}
		else if (tag_id == LXB_TAG_TITLE)
		{
			m_context_stack.top().in_title = false;
		}
		else if ((tag_id == LXB_TAG_SCRIPT || tag_id == LXB_TAG_IFRAME) && m_context_stack.top().in_script)
		{
			m_context_stack.top().in_script = false;
		}
		else if (tag_id == LXB_TAG_UL || tag_id == LXB_TAG_OL)
		{
			emit_tag(tag::CloseList{});
		}
		else if (tag_id == LXB_TAG_LI)
		{
			emit_tag(tag::CloseListItem{});
		}
		else if (tag_id == LXB_TAG_B)
		{
			emit_tag(tag::CloseBold{});
		}
		else if (tag_id == LXB_TAG_U)
		{
			emit_tag(tag::CloseUnderline{});
		}
    }
    else
    {
		if (tag_id == LXB_TAG_STYLE)
		{
			m_context_stack.top().in_style = true;
		}
		else if (paragraph_elements.contains(tag_id))
		{
			emit_tag(tag::Paragraph{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_DIV)
		{
			emit_tag(tag::Section{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_SPAN)
		{
			emit_tag(tag::Span{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_A)
		{
			const lxb_char_t* href_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t*)"href", 4, nullptr);
			std::string url;
			if (href_attr) {
				url = std::string((const char*)href_attr);
				//skip target if begins with "#" or "javascript"
				if (url[0] == '#' || url.find("javascript") == 0)
					url = "";
			}
			emit_tag(tag::Link{ .url = url, .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_IMG)
		{
			std::string src;
			const lxb_char_t* src_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"src", 3, nullptr);
			if (src_attr)
				src = std::string((const char*)src_attr);
			std::string alt;
			const lxb_char_t* alt_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"alt", 3, nullptr);
			if (alt_attr)
				alt = std::string((const char*)alt_attr);
			emit_tag(tag::Image{.src = src, .alt = alt, .styling = html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_TABLE)
		{
			emit_tag(tag::Table{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_CAPTION)
		{
			emit_tag(tag::Caption{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_TR)
		{
			emit_tag(tag::TableRow{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_TD || tag_id == LXB_TAG_TH)
		{
			emit_tag(tag::TableCell{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_UL || tag_id == LXB_TAG_OL)
		{
			bool style_type_none = false;
			const lxb_char_t* style_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t *)"style", 5, nullptr);
			if (style_attr)
			{
				std::string style_str((const char*)style_attr);
				if (style_str.find("list-style: none") != std::string::npos)
					style_type_none = true;
			}
			else if (tag_id == LXB_TAG_OL && m_context_stack.top().turn_off_ol_enumeration)
				style_type_none = true;
			else if (tag_id == LXB_TAG_UL && m_context_stack.top().turn_off_ul_enumeration)
				style_type_none = true;
			std::string list_type = style_type_none ? "none" : (tag_id == LXB_TAG_OL ? "decimal" : "disc");
			emit_tag(tag::List{.type = list_type, .styling=html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_BR)
		{
			m_context_stack.top().last_char_in_inline_formatting_context = '\0';
			emit_tag(tag::BreakLine{.styling=html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_LI)
		{
			emit_tag(tag::ListItem{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_TITLE)
		{
			m_context_stack.top().in_title = true;
		}
		else if (tag_id == LXB_TAG_SCRIPT || tag_id == LXB_TAG_IFRAME)
		{
			m_context_stack.top().in_script = true;
		}
		else if (tag_id == LXB_TAG_B)
		{
			emit_tag(tag::Bold{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_U)
		{
			emit_tag(tag::Underline{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_META)
		{
			const lxb_char_t* name_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t*)"name", 4, nullptr);
			const lxb_char_t* content_attr = lxb_dom_element_get_attribute(const_cast<lxb_dom_element_t*>(element), (const lxb_char_t*)"content", 7, nullptr);
			if (name_attr && content_attr)
			{
				std::string name((const char*)name_attr);
				std::string content((const char*)content_attr);
				// author, changedby, created, changed - LibreOffice 3.5
				// dcterms - old OpenOffice.org
				if (boost::iequals(name, "author") ||
						boost::iequals(name, "dcterms.creator"))
						m_context_stack.top().meta.author = content;
				else if (boost::iequals(name, "changedby") ||
						boost::iequals(name, "dcterms.contributor"))
				{
					// Multiple changedby meta tags are possible - LibreOffice 3.5 is an example
					if (!m_context_stack.top().meta.last_modified_by)
						m_context_stack.top().meta.last_modified_by = content;
				}
				else if (boost::iequals(name, "created") ||
						boost::iequals(name, "dcterms.issued"))
				{
					tm creation_date;
					if (string_to_date(content, creation_date))
					{
						m_context_stack.top().meta.creation_date = creation_date;
					}
				}
				else if (boost::iequals(name, "changed") ||
						boost::iequals(name, "dcterms.modified"))
				{
					// Multiple changed meta tags are possible - LibreOffice 3.5 is an example
					if (!m_context_stack.top().meta.last_modification_date)
					{
						tm last_modification_date;
						if (string_to_date(content, last_modification_date))
						{
							m_context_stack.top().meta.last_modification_date = last_modification_date;
						}
					}
				}
			}
		}
    }
}

HTMLParser::HTMLParser()
{
}

void HTMLParser::parse(const data_source& data, const emission_callbacks& emit_tag)
{
	docwire_log(debug) << "Using HTML parser.";
	std::string content = data.string();
	impl().parse_document(content, emit_tag);
	emit_tag(tag::CloseDocument{});
}

void HTMLParser::skipCharsetDecoding()
{
	impl().m_skip_decoding = true;
}

} // namespace docwire
