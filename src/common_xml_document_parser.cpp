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

#include "common_xml_document_parser.h"

#include "tags.h"
#include "zip_reader.h"
#include <libxml/xmlreader.h>
#include <functional>
#include <stack>
#include <variant>
#include "log.h"
#include "make_error.h"
#include "misc.h"
#include "xml_stream.h"
#include "xml_fixer.h"

namespace docwire
{

namespace
{

struct context
{
	const emission_callbacks& emit_tag;
	bool is_bold = false;
	bool is_italic = false;
	bool is_underline = false;
	bool space_preserve = false;
	bool stop_emmit_signals = false;
	size_t m_list_depth = 0;
	std::map<std::string, CommonXMLDocumentParser::ListStyleVector> m_list_styles;
	std::map<int, CommonXMLDocumentParser::Comment> m_comments;
	std::map<std::string, CommonXMLDocumentParser::Relationship> m_relationships;
	std::vector<CommonXMLDocumentParser::SharedString> m_shared_strings;
	bool m_disabled_text = false;
};

} // anonymous namespace

template<>
struct pimpl_impl<CommonXMLDocumentParser> : with_pimpl_owner<CommonXMLDocumentParser>
{
  template<typename T>
  CommonXMLDocumentParser::CommandHandler
  add_command_handler(T member_function)
  {
    return [this, member_function](XmlStream& xml_stream, XmlParseMode mode,
                                 ZipReader* zipfile, std::string& text,
                                 bool& children_processed, std::string& level_suffix, bool first_on_level)
    {
    	(this->*member_function)(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    };
  }

  pimpl_impl(CommonXMLDocumentParser& owner)
  : with_pimpl_owner{owner}
  {
    m_command_handlers["#text"] = add_command_handler<>(&pimpl_impl::onODFOOXMLText);
    m_command_handlers["b"] = add_command_handler<>(&pimpl_impl::onODFOOXMLBold);
    m_command_handlers["i"] = add_command_handler<>(&pimpl_impl::onODFOOXMLItalic);
    m_command_handlers["u"] = add_command_handler<>(&pimpl_impl::onODFOOXMLUnderline);
    m_command_handlers["p"] = add_command_handler<>(&pimpl_impl::onODFOOXMLPara);
    m_command_handlers["rPr"] = add_command_handler<>(&pimpl_impl::onrPr);
    m_command_handlers["pPr"] = add_command_handler<>(&pimpl_impl::onpPr);
    m_command_handlers["r"] = add_command_handler<>(&pimpl_impl::onR);
    m_command_handlers["tbl"] = add_command_handler<>(&pimpl_impl::onOOXMLTable);
    m_command_handlers["tr"] = add_command_handler<>(&pimpl_impl::onOOXMLTableRow);
    m_command_handlers["tc"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTableColumn);
    m_command_handlers["t"] = add_command_handler<>(&pimpl_impl::onODFOOXMLTextTag);
	m_command_handlers["text"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFText);
	m_command_handlers["tab"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFOOXMLTab);
	m_command_handlers["space"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFOOXMLSpace);
	m_command_handlers["s"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFOOXMLSpace);
	m_command_handlers["a"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFUrl);
	m_command_handlers["list-style"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFOOXMLListStyle);
	m_command_handlers["list"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFOOXMLList);
	m_command_handlers["table"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFTable);
	m_command_handlers["table-row"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFTableRow);
	m_command_handlers["table-cell"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFTableCell);
	m_command_handlers["annotation"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFAnnotation);
	m_command_handlers["line-break"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFLineBreak);
	m_command_handlers["h"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFHeading);
	m_command_handlers["object"] = add_command_handler<>(&pimpl_impl<CommonXMLDocumentParser>::onODFObject);
  }

	std::map<std::string, CommonXMLDocumentParser::CommandHandler> m_command_handlers;
	int m_xml_options;
  	std::stack<context> m_context_stack;

	continuation emit_tag(Tag&& tag) const
	{
		if (!m_context_stack.top().stop_emmit_signals || std::holds_alternative<std::exception_ptr>(tag))
			return m_context_stack.top().emit_tag(std::move(tag));
		else
			return continuation::proceed;
	}
	
	void reset_format()
	{
		m_context_stack.top().is_bold = false;
		m_context_stack.top().is_italic = false;
		m_context_stack.top().is_underline = false;
	}

  void onpPr(XmlStream& xml_stream, XmlParseMode mode,
           ZipReader* zipfile, std::string& text,
           bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;

    if (m_context_stack.top().is_underline)
    {
    	emit_tag(tag::CloseUnderline{});
    }
    if (m_context_stack.top().is_italic)
    {
    	emit_tag(tag::CloseItalic{});
    }
    if (m_context_stack.top().is_bold)
    {
    	emit_tag(tag::CloseBold{});
    }
    reset_format();
  }

	void onR(XmlStream& xml_stream, XmlParseMode mode,
	           ZipReader* zipfile, std::string& text,
	           bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		xml_stream.levelDown();
		text += owner().parseXmlData(xml_stream, mode, zipfile);
		xml_stream.levelUp();
		children_processed = true;

		if (m_context_stack.top().is_underline)
		{
			emit_tag(tag::CloseUnderline{});
		}
		if (m_context_stack.top().is_italic)
		{
			emit_tag(tag::CloseItalic{});
		}
		if (m_context_stack.top().is_bold)
		{
			emit_tag(tag::CloseBold{});
		}
		reset_format();
	}

  void onrPr(XmlStream& xml_stream, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    xml_stream.levelDown();
    owner().parseXmlData(xml_stream, mode, zipfile);
    if (m_context_stack.top().is_bold)
    {
    	emit_tag(tag::Bold{});
    }
    if (m_context_stack.top().is_italic)
    {
    	emit_tag(tag::Italic{});
    }
    if (m_context_stack.top().is_underline)
    {
    	emit_tag(tag::Underline{});
    }
    xml_stream.levelUp();
    children_processed = true;
  }

  void onODFOOXMLPara(XmlStream& xml_stream, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    emit_tag(tag::Paragraph{});
    docwire_log(debug) << "ODFOOXML_PARA command.";
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile) + '\n';
    xml_stream.levelUp();
    children_processed = true;
    emit_tag(tag::CloseParagraph{});
  }

  void onODFOOXMLText(XmlStream& xml_stream, XmlParseMode mode,
                        const ZipReader* zipfile, std::string& text,
                        bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    if (m_context_stack.top().m_disabled_text == false && xml_stream.name() == "#text")
    {
      char* content = xml_stream.content();
      if (content != NULL)
        text += content;
      children_processed = true;
      std::string s{content};
      if (m_context_stack.top().space_preserve || !std::all_of(s.begin(), s.end(), [](auto c){return isspace(static_cast<unsigned char>(c));}))
        emit_tag(tag::Text{.text = s});
    }
  }

  void onOOXMLTable(XmlStream& xml_stream, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    emit_tag(tag::Table{});
    docwire_log(debug) << "onOOXMLTable command.";
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    emit_tag(tag::CloseTable{});
  }

  void onOOXMLTableRow(XmlStream& xml_stream, XmlParseMode mode,
                       ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    emit_tag(tag::TableRow{});
    docwire_log(debug) << "onOOXMLTableRow command.";
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    emit_tag(tag::CloseTableRow{});
  }

  void onODFOOXMLTableColumn(XmlStream& xml_stream, XmlParseMode mode,
                       ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    emit_tag(tag::TableCell{});
    docwire_log(debug) << "onODFOOXMLTableColumn command.";
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    emit_tag(tag::CloseTableCell{});
  }

  void onODFOOXMLTextTag(XmlStream& xml_stream, XmlParseMode mode,
                      ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    docwire_log(debug) << "onODFOOXMLTextTag command.";
    xml_stream.levelDown();
    text += owner().parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
  }

  void onODFOOXMLBold(XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    m_context_stack.top().is_bold = xml_stream.attribute("val") != "false";
  }

  void onODFOOXMLItalic(XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    m_context_stack.top().is_italic = xml_stream.attribute("val") != "false";
  }

  void onODFOOXMLUnderline(XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    m_context_stack.top().is_underline = xml_stream.attribute("val") != "none";
  }

	void onUnregisteredCommand(XmlStream& xml_stream, XmlParseMode mode,
								const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		if (m_context_stack.top().m_disabled_text == false && xml_stream.name() == "#text")
		{
			char* content = xml_stream.content();
			if (content != NULL)
				text += content;
			children_processed = true;
		}
	}

	void executeCommand(const std::string& command, XmlStream& xml_stream, XmlParseMode mode,
						ZipReader* zipfile, std::string& text,
						bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		children_processed = false;
		std::map<std::string, CommonXMLDocumentParser::CommandHandler>::iterator it = m_command_handlers.find(command);
		if (it != m_command_handlers.end())
			it->second(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
		else
			onUnregisteredCommand(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	}

	void onODFText(XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_TEXT command.";
			char* content = xml_stream.content();
			if (content != NULL)
				text += content;
		}

	void onODFOOXMLTab(XmlStream& xml_stream, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_TAB command.";
			text += "\t";
			emit_tag(tag::Text{.text = "\t"});
		}

	void onODFOOXMLSpace(XmlStream& xml_stream, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_SPACE command.";
			std::string count_attr = xml_stream.attribute("c");
			int count = 1;
			if (!count_attr.empty())
				count = str_to_int(count_attr);
			for (int i = 0; i < count; i++)
			{
				text += " ";
			}
			emit_tag(tag::Text{.text = std::string(count, ' ')});
		}

	void onODFUrl(XmlStream& xml_stream, XmlParseMode mode,
								  ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_URL command.";
			std::string mlink = xml_stream.attribute("href");
			emit_tag(tag::Link{.url = mlink});
			xml_stream.levelDown();
			std::string text_link = owner().parseXmlData(xml_stream, mode, zipfile);
			text += formatUrl(mlink, text_link);
			xml_stream.levelUp();
			children_processed = true;
			emit_tag(tag::CloseLink{});
		}

	void onODFOOXMLListStyle(XmlStream& xml_stream, XmlParseMode mode,
										const ZipReader* zipfile, std::string& text,
										bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_LIST_STYLE command.";
			std::string style_code = xml_stream.attribute("name");
			if (!style_code.empty())
			{
				CommonXMLDocumentParser::ListStyleVector list_style(10, CommonXMLDocumentParser::bullet);
				xml_stream.levelDown();
				while (xml_stream)
				{
					std::string list_style_name = xml_stream.name();
					unsigned int level = strtol(xml_stream.attribute("level").c_str(), NULL, 10);
					if (level <= 10 && level > 0)
					{
						if (list_style_name == "list-level-style-number")
							list_style[level - 1] = CommonXMLDocumentParser::number;
						else
							list_style[level - 1] = CommonXMLDocumentParser::bullet;
					}
					xml_stream.next();
				}
				xml_stream.levelUp();
				children_processed = true;
				owner().getListStyles()[style_code] = list_style;
			}
		}

	void onODFOOXMLList(XmlStream& xml_stream, XmlParseMode mode,
								   ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			std::vector<std::string> list_vector;
			docwire_log(debug) << "ODFOOXML_LIST command.";
			owner().getListDepth()++;
			std::string header;
			CommonXMLDocumentParser::ODFOOXMLListStyle list_style = CommonXMLDocumentParser::bullet;
			std::string style_name = xml_stream.attribute("style-name");
			if (owner().getListDepth() <= 10 && !style_name.empty() && owner().getListStyles().find(style_name) != owner().getListStyles().end())
				list_style = owner().getListStyles()[style_name].at(owner().getListDepth() - 1);
			std::string list_type = (list_style == CommonXMLDocumentParser::number ? "decimal" : "disc");
			emit_tag(tag::List{.type = list_type});

			xml_stream.levelDown();
			while (xml_stream)
			{
				std::string list_element_type = xml_stream.name();
				xml_stream.levelDown();
				if (list_element_type == "list-header" && xml_stream)
				{
					header = owner().parseXmlData(xml_stream, mode, zipfile);
				}
				else if (xml_stream)
				{
					emit_tag(tag::ListItem{});
					list_vector.push_back(owner().parseXmlData(xml_stream, mode, zipfile));
				}

				emit_tag(tag::CloseListItem{});

				xml_stream.levelUp();
				xml_stream.next();
			}
			xml_stream.levelUp();

			if (header.length() > 0)
			{
				text += header;
				if (list_vector.size() > 0)
				{
					text += "\n";
					emit_tag(tag::BreakLine{});
				}
			}
			owner().getListDepth()--;
			emit_tag(tag::CloseList{});
			if (list_style == CommonXMLDocumentParser::number)
				text += formatNumberedList(list_vector);
			else
				text += formatList(list_vector);
			children_processed = true;
		}

	void onODFTable(XmlStream& xml_stream, XmlParseMode mode,
									ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			svector cell_vector;
			std::vector<svector> row_vector;
			docwire_log(debug) << "ODF_TABLE command.";
			emit_tag(tag::Table{});
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "table-row")
				{
					xml_stream.levelDown();
					cell_vector.clear();
					emit_tag(tag::TableRow{});
					while (xml_stream)
					{
						if (xml_stream.name() == "table-cell")
						{
							emit_tag(tag::TableCell{});
							xml_stream.levelDown();
							cell_vector.push_back(owner().parseXmlData(xml_stream, mode, zipfile));
							xml_stream.levelUp();
							emit_tag(tag::CloseTableCell{});
						}
						xml_stream.next();
					}
					row_vector.push_back(cell_vector);
					xml_stream.levelUp();
					emit_tag(tag::CloseTableRow{});
				}
				xml_stream.next();
			}
			xml_stream.levelUp();
			emit_tag(tag::CloseTable{});
			text += formatTable(row_vector);
			children_processed = true;
		}

		void onODFTableRow(XmlStream& xml_stream, XmlParseMode mode,
									   const ZipReader* zipfile, std::string& text,
									   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_ROW command.";
		}

		void onODFTableCell(XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_CELL command.";
		}

	void onODFAnnotation(XmlStream& xml_stream, XmlParseMode mode,
									ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_ANNOTATION command.";
			std::string creator;
			std::string date;
			std::string content;
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "creator")
					creator = xml_stream.stringValue();
				if (xml_stream.name() == "date")
					date = xml_stream.stringValue();
				if (xml_stream.name() == "p")
				{
					if (!content.empty())
						content += "\n";
					xml_stream.levelDown();
					owner().activeEmittingSignals(false);
					content += owner().parseXmlData(xml_stream, mode, zipfile);
					owner().activeEmittingSignals(true);
					xml_stream.levelUp();
				}
				xml_stream.next();
			}
			xml_stream.levelUp();
			emit_tag(tag::Comment{.author = creator, .time = date, .comment = content});
			text += owner().formatComment(creator, date, content);
			children_processed = true;
		}

	void onODFLineBreak(XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_LINE_BREAK command.";
			text += "\n";
			emit_tag(tag::BreakLine{});
		}

	void onODFHeading(XmlStream& xml_stream, XmlParseMode mode,
								 ZipReader* zipfile, std::string& text,
								 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_HEADING command.";
			xml_stream.levelDown();
			text += owner().parseXmlData(xml_stream, mode, zipfile) + '\n';
			xml_stream.levelUp();
			children_processed = true;
		}

	void onODFObject(XmlStream& xml_stream, XmlParseMode mode,
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_OBJECT command.";
			std::string href = xml_stream.attribute("href");
			std::string content_fn = (href.substr(0, 2) == "./" ? href.substr(2) : href) + "/content.xml";
			std::string content;
			if (!zipfile->read(content_fn, &content))
			{
				emit_tag(make_error_ptr("Error reading file", content_fn));
				return;
			}
			std::string object_text;
			try
			{
				owner().extractText(content, mode, zipfile, object_text);
			}
			catch (const std::exception&)
			{
				emit_tag(make_nested_ptr(std::current_exception(), make_error("Error parsing file", content_fn)));
			}
			text += object_text;
		}
};

void CommonXMLDocumentParser::activeEmittingSignals(bool flag)
{
	impl().m_context_stack.top().stop_emmit_signals = !flag;
}

void CommonXMLDocumentParser::registerODFOOXMLCommandHandler(const std::string& xml_tag, CommandHandler handler)
{
	impl().m_command_handlers[xml_tag] = handler;
}

std::string CommonXMLDocumentParser::parseXmlData(
  XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile)
{
	std::string text;
	std::string level_suffix;
	bool first_on_level = true;

	while (xml_stream)
	{
		bool space_preserve_prev = impl().m_context_stack.top().space_preserve;
		std::string space_attr = xml_stream.attribute("space");
		if (!space_attr.empty())
		{
			if (space_attr == "preserve")
    			impl().m_context_stack.top().space_preserve = true;
			else if (space_attr == "default")
				impl().m_context_stack.top().space_preserve = false;
		}
		bool children_processed;
		impl().executeCommand(xml_stream.name(), xml_stream, mode, zipfile, text,
			children_processed, level_suffix, first_on_level);
		if (xml_stream && (!children_processed))
		{
			xml_stream.levelDown();
			if (xml_stream)
				text += parseXmlData(xml_stream, mode, zipfile);
			xml_stream.levelUp();
		}
		impl().m_context_stack.top().space_preserve = space_preserve_prev;
		xml_stream.next();
		first_on_level = false;
	}
	if (!level_suffix.empty())
		text += level_suffix;
	return text;
}

void CommonXMLDocumentParser::extractText(const std::string& xml_contents, XmlParseMode mode, ZipReader* zipfile,
	std::string& text)
{
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
	std::string xml;
	if (mode == FIX_XML)
	{
		XmlFixer xml_fixer;
		xml = xml_fixer.fix(xml_contents);
	}
	else
		 xml = xml_contents;
	try
	{
		XmlStream xml_stream(xml, getXmlOptions());
		text = parseXmlData(xml_stream, mode, zipfile);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Parsing XML failed"));
	}
}

void CommonXMLDocumentParser::parseODFMetadata(const std::string &xml_content, attributes::Metadata& metadata) const
{
	try
	{
		XmlStream xml_stream(xml_content, XML_PARSE_NOBLANKS);
		xml_stream.levelDown();
		while (xml_stream)
		{
			if (xml_stream.name() == "meta")
			{
				xml_stream.levelDown();
				while (xml_stream)
				{
					if (xml_stream.name() == "initial-creator")
						metadata.author = xml_stream.stringValue();
					if (xml_stream.name() == "creation-date")
					{
						tm creation_date;
						string_to_date(xml_stream.stringValue(), creation_date);
						metadata.creation_date = creation_date;
					}
					if (xml_stream.name() == "creator")
						metadata.last_modified_by = xml_stream.stringValue();
					if (xml_stream.name() == "date")
					{
						tm last_modification_date;
						string_to_date(xml_stream.stringValue(), last_modification_date);
						metadata.last_modification_date = last_modification_date;
					}
					if (xml_stream.name() == "document-statistic")
					{
						std::string attr = xml_stream.attribute("meta::page-count"); // LibreOffice 3.5
						if (attr.empty())
							attr = xml_stream.attribute("page-count"); // older OpenOffice.org
						if (!attr.empty())
							metadata.page_count = str_to_int(attr);
						attr = xml_stream.attribute("meta:word-count"); // LibreOffice 3.5
						if (attr.empty())
							attr = xml_stream.attribute("word-count"); // older OpenOffice.org
						if (!attr.empty())
							metadata.word_count = str_to_int(attr);
					}
					xml_stream.next();
				}
				xml_stream.levelUp();
			}
			xml_stream.next();
		}
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error parsing ODF metadata"));
	}
}

const std::string CommonXMLDocumentParser::formatComment(const std::string& author, const std::string& time, const std::string& text)
{
	std::string comment = "\n[[[COMMENT BY " + author + " (" + time + ")]]]\n" + text;
	if (text.empty() || *text.rbegin() != '\n')
		comment += "\n";
	comment += "[[[---]]]\n";
	return comment;
}

size_t& CommonXMLDocumentParser::getListDepth()
{
	return impl().m_context_stack.top().m_list_depth;
}

std::map<std::string, CommonXMLDocumentParser::ListStyleVector>& CommonXMLDocumentParser::getListStyles()
{
	return impl().m_context_stack.top().m_list_styles;
}

std::map<int, CommonXMLDocumentParser::Comment>& CommonXMLDocumentParser::getComments()
{
	return impl().m_context_stack.top().m_comments;
}

std::map<std::string, CommonXMLDocumentParser::Relationship>& CommonXMLDocumentParser::getRelationships()
{
	return impl().m_context_stack.top().m_relationships;
}

std::vector<CommonXMLDocumentParser::SharedString>& CommonXMLDocumentParser::getSharedStrings()
{
	return impl().m_context_stack.top().m_shared_strings;
}

bool CommonXMLDocumentParser::disabledText() const
{
	return impl().m_context_stack.top().m_disabled_text;
}

void CommonXMLDocumentParser::disableText(bool disable)
{
	impl().m_context_stack.top().m_disabled_text = disable;
}

void CommonXMLDocumentParser::setXmlOptions(int options)
{
	impl().m_xml_options = options;
}

CommonXMLDocumentParser::CommonXMLDocumentParser()
{
		
		impl().m_xml_options = 0;
}

int CommonXMLDocumentParser::getXmlOptions() const
{
	return impl().m_xml_options;
}

CommonXMLDocumentParser::scoped_context_stack_push::scoped_context_stack_push(CommonXMLDocumentParser& parser, const emission_callbacks& emit_tag)
	: m_parser{parser}
{
	m_parser.impl().m_context_stack.push({emit_tag});
}

CommonXMLDocumentParser::scoped_context_stack_push::~scoped_context_stack_push()
{
	m_parser.impl().m_context_stack.pop();
}

} // namespace docwire
