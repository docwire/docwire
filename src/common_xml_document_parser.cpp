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

#include "common_xml_document_parser.h" 

#include "zip_reader.h"
#include <functional>
#include <type_traits>
#include <stack>
#include "log_scope.h"
#include "make_error.h"
#include "misc.h"
#include "nested_exception.h"
#include "xml_fixer.h"
#include "xml_root_element.h"
#include "document_elements.h"
#include "convert_chrono.h" // IWYU pragma: keep
#include "convert_numeric.h" // IWYU pragma: keep 
#include <algorithm>
#include "odf_ooxml_export.h"
#include "xml_attributes.h"

namespace docwire
{

namespace
{

template <safety_policy safety_level>
struct context
{
	const message_callbacks& emit_message;
	bool is_bold = false;
	bool is_italic = false;
	bool is_underline = false;
	bool space_preserve = false;
	bool stop_emit_signals = false;
	size_t m_list_depth = 0;
	std::map<std::string, typename CommonXMLDocumentParser<safety_level>::ListStyleVector> m_list_styles;
	std::map<int, typename CommonXMLDocumentParser<safety_level>::Comment> m_comments;
	std::map<std::string, typename CommonXMLDocumentParser<safety_level>::Relationship> m_relationships;
	std::vector<typename CommonXMLDocumentParser<safety_level>::SharedString> m_shared_strings;
	bool m_disabled_text = false;
};

} // anonymous namespace

template <safety_policy safety_level>
struct pimpl_impl<CommonXMLDocumentParser<safety_level>> : with_pimpl_owner<CommonXMLDocumentParser<safety_level>>
{
	using command_handler = CommonXMLDocumentParser<safety_level>::CommandHandler;
	using with_pimpl_owner<CommonXMLDocumentParser<safety_level>>::owner;

  template<typename T>
  command_handler add_command_handler(T member_function)
  {
    return [this, member_function](xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                                 ZipReader* zipfile, std::string& text,
                                 bool& children_processed, std::string& level_suffix, bool first_on_level)
    {
    	(this->*member_function)(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    };
  }

  pimpl_impl(CommonXMLDocumentParser<safety_level>& owner)
  : with_pimpl_owner<CommonXMLDocumentParser<safety_level>>{owner}
  {
    m_command_handlers["#text"] = add_command_handler<>(&pimpl_impl::onODFOOXMLText);
    m_command_handlers["b"] = add_command_handler<>(&pimpl_impl::onODFOOXMLBold);
    m_command_handlers["i"] = add_command_handler<>(&pimpl_impl::onODFOOXMLItalic);
    m_command_handlers["u"] = add_command_handler<>(&pimpl_impl::onODFOOXMLUnderline);
    m_command_handlers["p"] = add_command_handler<>(&pimpl_impl::onODFOOXMLPara);
    m_command_handlers["rPr"] = add_command_handler<>(&pimpl_impl::onrPr);
    m_command_handlers["pPr"] = add_command_handler<>(&pimpl_impl::onpPr);
    m_command_handlers["r"] = add_command_handler<>(&pimpl_impl::onR);
    m_command_handlers["tbl"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTable);
    m_command_handlers["tr"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTableRow);
    m_command_handlers["tc"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTableCell);
    m_command_handlers["t"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTextTag);
	m_command_handlers["text"] = add_command_handler<>(&pimpl_impl::onODFText);
	m_command_handlers["tab"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTab);
	m_command_handlers["space"] = add_command_handler<>(&pimpl_impl::onODFOOXMLSpace);
	m_command_handlers["s"] = add_command_handler<>(&pimpl_impl::onODFOOXMLSpace);
	m_command_handlers["a"] = add_command_handler<>(&pimpl_impl::onODFUrl);
	m_command_handlers["list-style"] = add_command_handler<>(&pimpl_impl::onODFOOXMLListStyle);
	m_command_handlers["list"] = add_command_handler<>(&pimpl_impl::onODFOOXMLList);
	m_command_handlers["table"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTable);
	m_command_handlers["table-row"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTableRow);
	m_command_handlers["table-cell"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTableCell);
	m_command_handlers["annotation"] = add_command_handler<>(&pimpl_impl::onODFAnnotation);
	m_command_handlers["line-break"] = add_command_handler<>(&pimpl_impl::onODFLineBreak);
	m_command_handlers["h"] = add_command_handler<>(&pimpl_impl::onODFHeading);
	m_command_handlers["object"] = add_command_handler<>(&pimpl_impl::onODFObject);
	m_command_handlers["fldData"] = add_command_handler<>(&pimpl_impl::onOOXMLFldData);
  }

	std::map<std::string, command_handler> m_command_handlers;
	xml::reader_blanks m_blanks = xml::reader_blanks::keep;
  	std::stack<context<safety_level>> m_context_stack;

	template <typename T>
	continuation emit_message(T&& object) const
	{
		log_scope(m_context_stack.top().stop_emit_signals);
		if (!m_context_stack.top().stop_emit_signals || std::is_same_v<std::decay_t<T>, std::exception_ptr>)
			return m_context_stack.top().emit_message(std::forward<T>(object));
		else
			return continuation::proceed;
	}

	void reset_format()
	{
		m_context_stack.top().is_bold = false;
		m_context_stack.top().is_italic = false;
		m_context_stack.top().is_underline = false;
	}

  void onpPr(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
           ZipReader* zipfile, std::string& text,
           bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
    text += owner().parseXmlChildren(xml_node, mode, zipfile);
    children_processed = true;

    if (m_context_stack.top().is_underline)
    {
    	emit_message(document::CloseUnderline{});
    }
    if (m_context_stack.top().is_italic)
    {
    	emit_message(document::CloseItalic{});
    }
    if (m_context_stack.top().is_bold)
    {
    	emit_message(document::CloseBold{});
    }
    reset_format();
  }

	void onR(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
	           ZipReader* zipfile, std::string& text,
	           bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		log_scope();
		text += owner().parseXmlChildren(xml_node, mode, zipfile);
		children_processed = true;

		if (m_context_stack.top().is_underline)
		{
			emit_message(document::CloseUnderline{});
		}
		if (m_context_stack.top().is_italic)
		{
			emit_message(document::CloseItalic{});
		}
		if (m_context_stack.top().is_bold)
		{
			emit_message(document::CloseBold{});
		}
		reset_format();
	}

  void onrPr(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
    reset_format();
    owner().parseXmlChildren(xml_node, mode, zipfile);
    if (m_context_stack.top().is_bold)
    {
    	emit_message(document::Bold{});
    }
    if (m_context_stack.top().is_italic)
    {
    	emit_message(document::Italic{});
    }
    if (m_context_stack.top().is_underline)
    {
    	emit_message(document::Underline{});
    }
    children_processed = true;
  }

  void onODFOOXMLPara(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
    reset_format();
    emit_message(document::Paragraph{});
    text += owner().parseXmlChildren(xml_node, mode, zipfile) + '\n';
    children_processed = true;
    emit_message(document::CloseParagraph{});
  }

  void onODFOOXMLText(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                        const ZipReader* zipfile, std::string& text,
                        bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
    if (m_context_stack.top().m_disabled_text == false)
    {
      std::string content { xml_node.content() };
	  log_entry(content);
	  text += content;
      children_processed = true;
      if (m_context_stack.top().space_preserve || !std::all_of(content.begin(), content.end(), [](auto c){return isspace(static_cast<unsigned char>(c));}))
        emit_message(document::Text{.text = content});
    }
  }

  void onODFOOXMLTable(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                 ZipReader* zipfile, std::string& text,
                 bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
	log_scope();
    reset_format();
    emit_message(document::Table{});
    text += owner().parseXmlChildren(xml_node, mode, zipfile);
    children_processed = true;
    emit_message(document::CloseTable{});
  }

  void onODFOOXMLTableRow(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                       ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
	log_scope();
    reset_format();
    emit_message(document::TableRow{});
    text += owner().parseXmlChildren(xml_node, mode, zipfile);
    children_processed = true;
    emit_message(document::CloseTableRow{});
  }

  void onODFOOXMLTableCell(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                       ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
	log_scope();
    reset_format();
    emit_message(document::TableCell{});
    text += owner().parseXmlChildren(xml_node, mode, zipfile);
    children_processed = true;
    emit_message(document::CloseTableCell{});
  }

  void onODFOOXMLTextTag(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
    text += owner().parseXmlChildren(xml_node, mode, zipfile);
    children_processed = true;
  }

  void onODFOOXMLBold(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
	m_context_stack.top().is_bold = attribute_value(xml_node, "val").value_or("") != "false";
  }

  void onODFOOXMLItalic(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
	m_context_stack.top().is_italic = attribute_value(xml_node, "val").value_or("") != "false";
  }

  void onODFOOXMLUnderline(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    log_scope();
	m_context_stack.top().is_underline = attribute_value(xml_node, "val").value_or("") != "none";
  }

	void onUnregisteredCommand(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		log_scope();
		// By default, children_processed is false, allowing the main loop to descend into children.
		// This is the desired behavior for unknown wrapper tags.
		// We only need to override this for non-element nodes (like comments or processing instructions)
		// that cannot have children, to prevent the parser from attempting to descend.
		if (xml_node.type() != xml::node_type::element)
		{
			children_processed = true;
		}
	}

	void executeCommand(const std::string& command, xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
						ZipReader* zipfile, std::string& text,
						bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		log_scope(command);
		children_processed = false;
		typename std::map<std::string, command_handler>::iterator it = m_command_handlers.find(command);
		if (it != m_command_handlers.end())
			it->second(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
		else
			onUnregisteredCommand(xml_node, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	}

	void onODFText(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			text += xml_node.content();
		}

	void onODFOOXMLTab(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			text += "\t"; 
			emit_message(document::Text{.text = "\t"});
		}

	void onODFOOXMLSpace(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			int count = xml::attribute_value<int>(xml_node, "c").value_or(1);
			for (int i = 0; i < count; i++)
			{
				text += " ";
			}
			emit_message(document::Text{.text = std::string(count, ' ')});
		}

	void onODFUrl(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								  ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			std::string mlink { attribute_value(xml_node, "href").value_or("") };
			emit_message(document::Link{.url = mlink});
			std::string text_link = owner().parseXmlChildren(xml_node, mode, zipfile);
			text += formatUrl(mlink, text_link);
			children_processed = true;
			emit_message(document::CloseLink{});
		}

	void onODFOOXMLListStyle(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
										const ZipReader* zipfile, std::string& text,
										bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			std::string style_code { attribute_value(xml_node, "name").value_or("") };
			if (!style_code.empty())
			{
				typename CommonXMLDocumentParser<safety_level>::ListStyleVector list_style(10, CommonXMLDocumentParser<safety_level>::bullet);
				for (auto node: children(xml_node))
				{
					std::string_view list_style_name = node.name();
					unsigned int level = attribute_value<unsigned int>(node, "level").value_or(1);
					if (level <= 10 && level > 0)
					{
						if (list_style_name == "list-level-style-number")
							list_style[level - 1] = CommonXMLDocumentParser<safety_level>::number;
						else
							list_style[level - 1] = CommonXMLDocumentParser<safety_level>::bullet;
					}
				}
				children_processed = true;
				owner().getListStyles()[style_code] = list_style;
			}
		}

	void onODFOOXMLList(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								   ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			std::vector<std::string> list_vector;
			owner().getListDepth()++;
			std::string header;
			typename CommonXMLDocumentParser<safety_level>::ODFOOXMLListStyle list_style = CommonXMLDocumentParser<safety_level>::bullet;
			std::string style_name { attribute_value(xml_node, "style-name").value_or("") };
			if (owner().getListDepth() <= 10 && !style_name.empty() && owner().getListStyles().find(style_name) != owner().getListStyles().end())
				list_style = owner().getListStyles()[style_name].at(owner().getListDepth() - 1);
			std::string list_type = (list_style == CommonXMLDocumentParser<safety_level>::number ? "decimal" : "disc");
			emit_message(document::List{.type = list_type});

			for (auto node: children(xml_node))
			{
				std::string_view list_element_type = node.name();
				if (list_element_type == "list-header")
				{
					header = owner().parseXmlChildren(node, mode, zipfile);
				}
				else
				{
					emit_message(document::ListItem{});
					list_vector.push_back(owner().parseXmlChildren(node, mode, zipfile));
				}

				emit_message(document::CloseListItem{});
			}

			if (header.length() > 0)
			{
				text += header;
				if (list_vector.size() > 0)
				{
					text += "\n";
					emit_message(document::BreakLine{});
				}
			}
			owner().getListDepth()--;
			emit_message(document::CloseList{});
			if (list_style == CommonXMLDocumentParser<safety_level>::number)
				text += formatNumberedList(list_vector);
			else
				text += formatList(list_vector);
			children_processed = true;
		}

	void onODFAnnotation(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
									ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			std::string creator;
			std::string date;
			std::string content;
			for (auto node: children(xml_node))
			{
				if (node.name() == "creator")
					creator = node.string_value();
				if (node.name() == "date")
					date = node.string_value();
				if (node.name() == "p")
				{
					if (!content.empty())
						content += "\n";
					owner().activeEmittingSignals(false);
					content += owner().parseXmlChildren(node, mode, zipfile);
					owner().activeEmittingSignals(true);
				}
			}
			emit_message(document::Comment{.author = creator, .time = date, .comment = content});
			text += owner().formatComment(creator, date, content);
			children_processed = true;
		}

	void onODFLineBreak(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			text += "\n";
			emit_message(document::BreakLine{});
		}

	void onODFHeading(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								 ZipReader* zipfile, std::string& text,
								 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			text += owner().parseXmlChildren(xml_node, mode, zipfile) + '\n';
			children_processed = true;
		}

	void onODFObject(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			log_scope();
			std::string_view href = attribute_value(xml_node, "href").value_or("");
			std::string content_fn = std::string{href.substr(0, 2) == "./" ? href.substr(2) : href} + "/content.xml";
			std::string content;
			if (!zipfile->read(content_fn, &content))
			{
				emit_message(make_error_ptr("Error reading file", content_fn));
				return;
			}
			std::string object_text;
			try
			{
				owner().extractText(content, mode, zipfile, object_text);
			}
			catch (const std::exception&)
			{
				emit_message(make_nested_ptr(std::current_exception(), make_error("Error parsing file", content_fn)));
			}
			text += object_text;
		}

	void onOOXMLFldData(xml::node_ref<safety_level>& xml_node, XmlParseMode mode,
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		log_scope();
		children_processed = true;
	}
};

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::activeEmittingSignals(bool flag)
{
	log_scope(flag);
	impl().m_context_stack.top().stop_emit_signals = !flag;
}

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::registerODFOOXMLCommandHandler(const std::string& xml_tag, const CommandHandler& handler)
{
	impl().m_command_handlers[xml_tag] = handler;
}

template <safety_policy safety_level>
std::string CommonXMLDocumentParser<safety_level>::parseXmlData(xml::children_view<safety_level> xml_nodes, XmlParseMode mode, ZipReader* zipfile)
{
	log_scope();
	std::string text;
	std::string level_suffix;
	bool first_on_level = true;

	for (auto node: xml_nodes)
	{
		bool space_preserve_prev = impl().m_context_stack.top().space_preserve;
		std::string_view space_attr = attribute_value(node, "space").value_or("");
		if (!space_attr.empty())
		{
			if (space_attr == "preserve")
    			impl().m_context_stack.top().space_preserve = true;
			else if (space_attr == "default")
				impl().m_context_stack.top().space_preserve = false;
		}
		bool children_processed;
		impl().executeCommand(std::string{node.name()}, node, mode, zipfile, text,
			children_processed, level_suffix, first_on_level);
		if (!children_processed)
		{
			text += parseXmlChildren(node, mode, zipfile);
		}
		impl().m_context_stack.top().space_preserve = space_preserve_prev;
		first_on_level = false;
	}
	if (!level_suffix.empty())
		text += level_suffix;
	return text;
}

template <safety_policy safety_level>
std::string CommonXMLDocumentParser<safety_level>::parseXmlChildren(
  xml::node_ref<safety_level>& xml_node, XmlParseMode mode, ZipReader* zipfile)
{
	log_scope();
	std::string text;
	text = parseXmlData(children(xml_node), mode, zipfile);
	return text;
}

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::extractText(std::string_view xml_contents, XmlParseMode mode, ZipReader* zipfile,
	std::string& text)
{
	log_scope();
	if (mode == STRIP_XML)
	{
		text = "";
		bool in_tag = false;
		for (int i = 0; i < xml_contents.length(); i++)
		{
			char ch = xml_contents[i];
			if (ch == '<')
				in_tag = true;
			else if (ch == '>')
				in_tag = false;
			else if (!in_tag)
				text += ch;
		}
		return;
	}
	try
	{
		if (mode == FIX_XML)
		{
			XmlFixer xml_fixer;
			std::string fixed_xml = xml_fixer.fix(std::string{xml_contents});
			xml::reader<safety_level> reader(std::move(fixed_xml), blanks());
			text = parseXmlData(children(reader), mode, zipfile);
		}
		else
		{
			xml::reader<safety_level> xml_reader(xml_contents, blanks());
			text = parseXmlData(children(xml_reader), mode, zipfile);
		}
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Parsing XML failed"));
	}
}

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::parseODFMetadata(std::string_view xml_content, attributes::Metadata& metadata) const
{
	log_scope();
	try
	{
		xml::reader<safety_level> xml_reader(xml_content, xml::reader_blanks::ignore);
		for (auto sub_node: children(root_element(xml_reader)))
		{
			if (sub_node.name() == "meta")
			{
				for (auto node: children(sub_node))
				{
					if (node.name() == "initial-creator")
						metadata.author = node.string_value();
					if (node.name() == "creation-date")
                    	metadata.creation_date = convert::try_to<std::chrono::sys_seconds>(with::date_format::iso8601{node.string_value()});
					if (node.name() == "creator")
						metadata.last_modified_by = node.string_value();
					if (node.name() == "date")
						metadata.last_modification_date = convert::try_to<std::chrono::sys_seconds>(with::date_format::iso8601{node.string_value()});
					if (node.name() == "document-statistic")
					{
						metadata.page_count = attribute_value<int>(node, "meta:page-count").unwrap(); // LibreOffice 3.5
						if (!metadata.page_count)
							metadata.page_count = attribute_value<int>(node, "page-count").unwrap(); // older OpenOffice.org
						metadata.word_count = attribute_value<int>(node, "meta:word-count").unwrap(); // LibreOffice 3.5
						if (!metadata.word_count)
							metadata.word_count = attribute_value<int>(node, "word-count").unwrap(); // older OpenOffice.org
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error parsing ODF metadata"));
	}
}

template <safety_policy safety_level>
const std::string CommonXMLDocumentParser<safety_level>::formatComment(const std::string& author, const std::string& time, const std::string& text)
{
	log_scope(author, time, text);
	std::string comment = "\n[[[COMMENT BY " + author + " (" + time + ")]]]\n" + text;
	if (text.empty() || *text.rbegin() != '\n')
		comment += "\n";
	comment += "[[[---]]]\n";
	return comment;
}

template <safety_policy safety_level>
size_t& CommonXMLDocumentParser<safety_level>::getListDepth()
{
	return impl().m_context_stack.top().m_list_depth;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::ListStyleMap& CommonXMLDocumentParser<safety_level>::getListStyles()
{
	return impl().m_context_stack.top().m_list_styles;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::CommentMap& CommonXMLDocumentParser<safety_level>::getComments()
{
	return impl().m_context_stack.top().m_comments;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::RelationshipMap& CommonXMLDocumentParser<safety_level>::getRelationships()
{
	return impl().m_context_stack.top().m_relationships;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::SharedStringVector& CommonXMLDocumentParser<safety_level>::getSharedStrings()
{
	return impl().m_context_stack.top().m_shared_strings;
}

template <safety_policy safety_level>
bool CommonXMLDocumentParser<safety_level>::disabledText() const
{
	return impl().m_context_stack.top().m_disabled_text;
}

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::disableText(bool disable)
{
	impl().m_context_stack.top().m_disabled_text = disable;
}

template <safety_policy safety_level>
void CommonXMLDocumentParser<safety_level>::set_blanks(xml::reader_blanks blanks)
{
	impl().m_blanks = blanks;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::CommonXMLDocumentParser()
{
}

template <safety_policy safety_level>
xml::reader_blanks CommonXMLDocumentParser<safety_level>::blanks() const
{
	return impl().m_blanks;
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::scoped_context_stack_push::scoped_context_stack_push(CommonXMLDocumentParser& parser, const message_callbacks& emit_message)
	: m_parser{parser}
{
	log_scope();
	m_parser.impl().m_context_stack.push({emit_message});
}

template <safety_policy safety_level>
CommonXMLDocumentParser<safety_level>::scoped_context_stack_push::~scoped_context_stack_push()
{
	log_scope();
	m_parser.impl().m_context_stack.pop();
}

template class DOCWIRE_ODF_OOXML_EXPORT CommonXMLDocumentParser<strict>;
template class DOCWIRE_ODF_OOXML_EXPORT CommonXMLDocumentParser<relaxed>;

// These are not required but help static analyzer
template class pimpl_impl<CommonXMLDocumentParser<strict>>;
template class pimpl_impl<CommonXMLDocumentParser<relaxed>>;

} // namespace docwire
