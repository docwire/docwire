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
	if (charset == "utf-8" && charset == "UTF-8")
		return;
	charset_converter converter(charset, "UTF-8");
	text = converter.convert(text);
}

std::optional<std::string> get_encoding_from_meta_tags(const std::string& html_content)
{
	lxb_html_encoding_t em;
	throw_if (lxb_html_encoding_init(&em) != LXB_STATUS_OK);
	if (lxb_html_encoding_determine(&em, (const lxb_char_t*)html_content.data(), (lxb_char_t*)html_content.data() + html_content.length()) != LXB_STATUS_OK)
		return std::nullopt;
	lxb_html_encoding_entry_t* entry = lxb_html_encoding_meta_entry(&em, 0);
	if (entry == nullptr)
		return std::nullopt;
	std::string detected_encoding = std::string((const char*)entry->name, (size_t)(entry->end - entry->name));
	lxb_html_encoding_destroy(&em, false);
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

struct state
{
	bool turn_off_ul_enumeration = false;
	bool turn_off_ol_enumeration = false;
	bool head_parsed = false;
	bool in_metadata = false;
	bool in_head = false;
	bool in_script = false;
	bool in_style = false;
	std::string style_text;
	attributes::Metadata meta;
	std::string buffered_text;
	char last_char_in_inline_formatting_context = '\0';
	state() = default;
	state(state&&) = default;
	state& operator=(state&&) = default;
	state(const state&) = delete;
	state& operator=(const state&) = delete;
};

} // unnamed namespace

template<>
struct pimpl_impl<HTMLParser> : with_pimpl_owner<HTMLParser>
{
	pimpl_impl(HTMLParser& owner) : with_pimpl_owner(owner) {}
	void parse_css(const std::string & m_style_text);
	void parse_document(const std::string& html_content);
	void process_node(const lxb_dom_node_t* node);
	void process_text(const lxb_dom_node_t* node);
	void process_tag(const lxb_dom_node_t* node, bool is_closing = false);
	bool m_skip_decoding = false;
	state m_state;
};

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
				m_state.turn_off_ol_enumeration = true;
				m_state.turn_off_ul_enumeration = true;
			}
			else if (match == "ul")
				m_state.turn_off_ul_enumeration = true;
			else if (match == "ol")
				m_state.turn_off_ol_enumeration = true;
		}
	}
}

void pimpl_impl<HTMLParser>::parse_document(const std::string& html_content_arg)
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
			owner().sendTag(make_nested_ptr(std::current_exception(), make_error("Error ensuring HTML is encoded in UTF-8")));
		}
	}

	m_state = state{};

	lxb_html_document_t* document = lxb_html_document_create();
	throw_if(document == nullptr, "lxb_html_document_create failed");

	throw_if(lxb_html_document_parse(document, (const lxb_char_t *)html_content.data(), html_content.size()) != LXB_STATUS_OK,
		"Failed to parse HTML document");

	lxb_dom_node_t* head = lxb_dom_interface_node(lxb_html_document_head_element(document));
	parse_css((const char*)lxb_dom_node_text_content(head, nullptr));

	owner().sendTag(tag::Document
		{
			.metadata = [this, head]()
			{
				docwire_log(debug) << "Extracting metadata.";
				if (!m_state.head_parsed)
				{
					m_state.in_metadata = true;
					m_state.in_head = true;
					process_node(head);
					m_state.in_head = false;
					m_state.in_metadata = false;
					m_state.head_parsed = true;
				}
				return m_state.meta;
			}
		});

	m_state.in_head = true;
	process_node(head);
	m_state.in_head = false;

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
			if (m_state.in_metadata || m_state.in_script)
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
	if (m_state.in_head && !m_state.in_style)
		return;
    const lxb_char_t* data = lxb_dom_node_text_content(const_cast<lxb_dom_node_t*>(node), nullptr);
    if (data == nullptr)
        return;
    std::string text((const char*)data);
	if (m_state.in_style)
	{
		m_state.style_text += text;
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
	bool last_char_was_space = isspace((unsigned char)m_state.last_char_in_inline_formatting_context);
	docwire_log(debug) << "Last char in inline formatting context was whitespace: " << last_char_was_space;
	// Reduce whitespaces between text nodes (end of previous and beginning of current.
	// Remove whitespaces from beginning of inline formatting context.
	if (last_char_was_space || m_state.last_char_in_inline_formatting_context == '\0')
	{
		boost::trim_left(text);
		docwire_log(debug) << "After reducing whitespaces between text nodes and removing whitespaces from begining of inline formatting context: [" << text << "]";
	}
	if (!text.empty())
	{
		m_state.last_char_in_inline_formatting_context = text.back();
		// buffer text because whitespaces from end of inline formatting context should be removed.
		m_state.buffered_text += text;
	}
}

void pimpl_impl<HTMLParser>::process_tag(const lxb_dom_node_t* node, bool is_closing)
{
    const lxb_dom_element_t* element = lxb_dom_interface_element(node);
    lxb_tag_id_t tag_id = lxb_dom_element_tag_id(const_cast<lxb_dom_element_t*>(element));
	if (!m_state.buffered_text.empty())
	{
		// https://developer.mozilla.org/en-US/docs/Web/HTML/Block-level_elements
		// TODO: elements can have also block style in CSS
		static std::set<lxb_tag_id_t> block_level_elements = { LXB_TAG_ADDRESS, LXB_TAG_ARTICLE, LXB_TAG_ASIDE, LXB_TAG_BLOCKQUOTE, LXB_TAG_DETAILS, LXB_TAG_DIALOG, LXB_TAG_DD, LXB_TAG_DIV, LXB_TAG_DL,
			LXB_TAG_DT, LXB_TAG_FIELDSET, LXB_TAG_FIGCAPTION, LXB_TAG_FIGURE, LXB_TAG_FOOTER, LXB_TAG_FORM, LXB_TAG_H1, LXB_TAG_H2, LXB_TAG_H3, LXB_TAG_H4, LXB_TAG_H5, LXB_TAG_H6, LXB_TAG_HEADER, LXB_TAG_HGROUP, LXB_TAG_HR, LXB_TAG_LI,
			LXB_TAG_MAIN, LXB_TAG_NAV, LXB_TAG_OL, LXB_TAG_P, LXB_TAG_PRE, LXB_TAG_SECTION, LXB_TAG_TABLE, LXB_TAG_UL };
		// We treat the following elements like block-level elements. It should be more clear when CSS support is better.
		static std::set<lxb_tag_id_t> similar_to_block_level_elements = { LXB_TAG_HTML, LXB_TAG_BODY, LXB_TAG_TD, LXB_TAG_TR, LXB_TAG_TH };		
		if (block_level_elements.contains(tag_id) || similar_to_block_level_elements.contains(tag_id))
		{
			// Remove sequences of spaces at the end of an block-level element
			// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace
			boost::trim_right(m_state.buffered_text);
			m_state.last_char_in_inline_formatting_context = '\0'; // inline formatting context is now empty
		}
		if (!m_state.buffered_text.empty())
		{
			owner().sendTag(tag::Text{.text = m_state.buffered_text});
			m_state.buffered_text = "";
		}
	}
    // All html elements that will emit tag::Paragraph (as we do not have H1 etc)
	std::set<lxb_tag_id_t> paragraph_elements = { LXB_TAG_H1, LXB_TAG_H2, LXB_TAG_H3, LXB_TAG_H4, LXB_TAG_H5, LXB_TAG_H6, LXB_TAG_P };	
    if (is_closing)
    {
		if (tag_id == LXB_TAG_STYLE)
		{
			m_state.in_style = false;
			if (!m_state.in_metadata)
				owner().sendTag(tag::Style{.css_text = m_state.style_text});
			m_state.style_text.clear();
		}
		else if (paragraph_elements.contains(tag_id))
		{
			owner().sendTag(tag::CloseParagraph{});
		}
		else if (tag_id == LXB_TAG_DIV)
		{
			owner().sendTag(tag::CloseSection{});
		}
		else if (tag_id == LXB_TAG_SPAN)
		{
			owner().sendTag(tag::CloseSpan{});
		}
		else if (tag_id == LXB_TAG_A)
		{
			owner().sendTag(tag::CloseLink{});
		}
		else if (tag_id == LXB_TAG_TABLE)
		{
			owner().sendTag(tag::CloseTable{});
		}
		else if (tag_id == LXB_TAG_CAPTION)
		{
			owner().sendTag(tag::CloseCaption{});
		}
		else if (tag_id == LXB_TAG_TR)
		{
			owner().sendTag(tag::CloseTableRow{});
		}
		else if (tag_id == LXB_TAG_TD || tag_id == LXB_TAG_TH)
		{
			owner().sendTag(tag::CloseTableCell{});
		}
		else if ((tag_id == LXB_TAG_SCRIPT || tag_id == LXB_TAG_IFRAME) && m_state.in_script)
		{
			m_state.in_script = false;
		}
		else if (tag_id == LXB_TAG_UL || tag_id == LXB_TAG_OL)
		{
			owner().sendTag(tag::CloseList{});
		}
		else if (tag_id == LXB_TAG_LI)
		{
			owner().sendTag(tag::CloseListItem{});
		}
		else if (tag_id == LXB_TAG_B)
		{
			owner().sendTag(tag::CloseBold{});
		}
		else if (tag_id == LXB_TAG_U)
		{
			owner().sendTag(tag::CloseUnderline{});
		}
    }
    else
    {
		if (tag_id == LXB_TAG_STYLE)
		{
			m_state.in_style = true;
		}
		else if (paragraph_elements.contains(tag_id))
		{
			owner().sendTag(tag::Paragraph{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_DIV)
		{
			owner().sendTag(tag::Section{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_SPAN)
		{
			owner().sendTag(tag::Span{ .styling = html_node_styling(node) });
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
			owner().sendTag(tag::Link{ .url = url, .styling = html_node_styling(node) });
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
			owner().sendTag(tag::Image{.src = src, .alt = alt, .styling = html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_TABLE)
		{
			owner().sendTag(tag::Table{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_CAPTION)
		{
			owner().sendTag(tag::Caption{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_TR)
		{
			owner().sendTag(tag::TableRow{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_TD || tag_id == LXB_TAG_TH)
		{
			owner().sendTag(tag::TableCell{ .styling = html_node_styling(node) });
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
			else if (tag_id == LXB_TAG_OL && m_state.turn_off_ol_enumeration)
				style_type_none = true;
			else if (tag_id == LXB_TAG_UL && m_state.turn_off_ul_enumeration)
				style_type_none = true;
			std::string list_type = style_type_none ? "none" : (tag_id == LXB_TAG_OL ? "decimal" : "disc");
			owner().sendTag(tag::List{.type = list_type, .styling=html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_BR)
		{
			m_state.last_char_in_inline_formatting_context = '\0';
			owner().sendTag(tag::BreakLine{.styling=html_node_styling(node)});
		}
		else if (tag_id == LXB_TAG_LI)
		{
			owner().sendTag(tag::ListItem{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_SCRIPT || tag_id == LXB_TAG_IFRAME)
		{
			m_state.in_script = true;
		}
		else if (tag_id == LXB_TAG_B)
		{
			owner().sendTag(tag::Bold{ .styling = html_node_styling(node) });
		}
		else if (tag_id == LXB_TAG_U)
		{
			owner().sendTag(tag::Underline{ .styling = html_node_styling(node) });
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
						m_state.meta.author = content;
				else if (boost::iequals(name, "changedby") ||
						boost::iequals(name, "dcterms.contributor"))
				{
					// Multiple changedby meta tags are possible - LibreOffice 3.5 is an example
					if (!m_state.meta.last_modified_by)
						m_state.meta.last_modified_by = content;
				}
				else if (boost::iequals(name, "created") ||
						boost::iequals(name, "dcterms.issued"))
				{
					tm creation_date;
					if (string_to_date(content, creation_date))
					{
						m_state.meta.creation_date = creation_date;
					}
				}
				else if (boost::iequals(name, "changed") ||
						boost::iequals(name, "dcterms.modified"))
				{
					// Multiple changed meta tags are possible - LibreOffice 3.5 is an example
					if (!m_state.meta.last_modification_date)
					{
						tm last_modification_date;
						if (string_to_date(content, last_modification_date))
						{
							m_state.meta.last_modification_date = last_modification_date;
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

void
HTMLParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using HTML parser.";
	std::string content = data.string();
	impl().parse_document(content);
	sendTag(tag::CloseDocument{});
}

void HTMLParser::skipCharsetDecoding()
{
	impl().m_skip_decoding = true;
}

} // namespace docwire
