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

#include "zip_reader.h"
#include "exception.h"
#include <iostream>
#include <libxml/xmlreader.h>
#include <functional>
#include "log.h"
#include "misc.h"
#include "xml_stream.h"
#include "xml_fixer.h"
#include <boost/signals2.hpp>

namespace docwire
{

struct CommonXMLDocumentParser::Implementation
{
  template<typename T>
  CommandHandler
  add_command_handler(T member_function)
  {
    return [this, member_function](CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                                 const ZipReader* zipfile, std::string& text,
                                 bool& children_processed, std::string& level_suffix, bool first_on_level)
    {
      (this->*member_function)(parser, xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    };
  }

  Implementation()
  : is_bold(false),
    is_italic(false),
    is_underline(false),
    space_preserve(false),
    stop_emmit_signals(false)
  {
    m_command_handlers["#text"] = add_command_handler<>(&Implementation::onODFOOXMLText);
    m_command_handlers["b"] = add_command_handler<>(&Implementation::onODFOOXMLBold);
    m_command_handlers["i"] = add_command_handler<>(&Implementation::onODFOOXMLItalic);
    m_command_handlers["u"] = add_command_handler<>(&Implementation::onODFOOXMLUnderline);
    m_command_handlers["p"] = add_command_handler<>(&Implementation::onODFOOXMLPara);
    m_command_handlers["rPr"] = add_command_handler<>(&Implementation::onrPr);
    m_command_handlers["pPr"] = add_command_handler<>(&Implementation::onpPr);
    m_command_handlers["r"] = add_command_handler<>(&Implementation::onR);
    m_command_handlers["tbl"] = add_command_handler<>(&Implementation::onODFOOXMLTable);
    m_command_handlers["tr"] = add_command_handler<>(&Implementation::onODFOOXMLTableRow);
    m_command_handlers["tc"] = add_command_handler<>(&Implementation::onODFOOXMLTableColumn);
    m_command_handlers["t"] = add_command_handler<>(&Implementation::onODFOOXMLTextTag);
  }

	bool is_bold;
	bool is_italic;
	bool is_underline;
	bool space_preserve;
	bool stop_emmit_signals;
	size_t m_list_depth;
	std::map<std::string, ListStyleVector> m_list_styles;
	std::map<int, Comment> m_comments;
	std::vector<SharedString> m_shared_strings;
	std::map<std::string, CommandHandler> m_command_handlers;
	boost::signals2::signal<void(Info &info)> m_on_new_node_signal;

	bool m_disabled_text;
	int m_xml_options;
	CommonXMLDocumentParser* m_parser;

	void send_tag(const Tag& tag)
	{
		if (!stop_emmit_signals)
		{
			Info info(tag);
			m_on_new_node_signal(info);
		}
	}

	void reset_format()
	{
		is_bold = false;
		is_italic = false;
		is_underline = false;
	}

  void onpPr(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
           const ZipReader* zipfile, std::string& text,
           bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;

    if (is_underline)
    {
      send_tag(tag::CloseUnderline{});
    }
    if (is_italic)
    {
      send_tag(tag::CloseItalic{});
    }
    if (is_bold)
    {
      send_tag(tag::CloseBold{});
    }
    reset_format();
  }

	void onR(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
	           const ZipReader* zipfile, std::string& text,
	           bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		xml_stream.levelDown();
		text += parser.parseXmlData(xml_stream, mode, zipfile);
		xml_stream.levelUp();
		children_processed = true;

		if (is_underline)
		{
			send_tag(tag::CloseUnderline{});
		}
		if (is_italic)
		{
			send_tag(tag::CloseItalic{});
		}
		if (is_bold)
		{
			send_tag(tag::CloseBold{});
		}
		reset_format();
	}

  void onrPr(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    xml_stream.levelDown();
    parser.parseXmlData(xml_stream, mode, zipfile);
    if (is_bold)
    {
      send_tag(tag::Bold{});
    }
    if (is_italic)
    {
      send_tag(tag::Italic{});
    }
    if (is_underline)
    {
      send_tag(tag::Underline{});
    }
    xml_stream.levelUp();
    children_processed = true;
  }

  void onODFOOXMLPara(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    send_tag(tag::Paragraph{});
    docwire_log(debug) << "ODFOOXML_PARA command.";
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile) + '\n';
    xml_stream.levelUp();
    children_processed = true;
    send_tag(tag::CloseParagraph{});
  }

  void onODFOOXMLText(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                        const ZipReader* zipfile, std::string& text,
                        bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    if (m_disabled_text == false && xml_stream.name() == "#text")
    {
      char* content = xml_stream.content();
      if (content != NULL)
        text += content;
      children_processed = true;
      std::string s{content};
      if (space_preserve || !std::all_of(s.begin(), s.end(), [](auto c){return isspace(static_cast<unsigned char>(c));}))
        send_tag(tag::Text{.text = s});
    }
  }

  void onODFOOXMLTable(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    send_tag(tag::Table{});
    docwire_log(debug) << "onODFOOXMLTable command.";
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    send_tag(tag::CloseTable{});
  }

  void onODFOOXMLTableRow(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                       const ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    send_tag(tag::TableRow{});
    docwire_log(debug) << "onODFOOXMLTableRow command.";
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    send_tag(tag::CloseTableRow{});
  }

  void onODFOOXMLTableColumn(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                       const ZipReader* zipfile, std::string& text,
                       bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    reset_format();
    send_tag(tag::TableCell{});
    docwire_log(debug) << "onODFOOXMLTableColumn command.";
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
    send_tag(tag::CloseTableCell{});
  }

  void onODFOOXMLTextTag(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    docwire_log(debug) << "onODFOOXMLTextTag command.";
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    children_processed = true;
  }

  void onODFOOXMLBold(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    is_bold = xml_stream.attribute("val") != "false";
  }

  void onODFOOXMLItalic(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    is_italic = xml_stream.attribute("val") != "false";
  }

  void onODFOOXMLUnderline(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                      const ZipReader* zipfile, std::string& text,
                      bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    is_underline = xml_stream.attribute("val") != "none";
  }

	void executeCommand(const std::string& command, XmlStream& xml_stream, XmlParseMode mode,
						const ZipReader* zipfile, std::string& text,
						bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		children_processed = false;
		std::map<std::string, CommonXMLDocumentParser::CommandHandler>::iterator it = m_command_handlers.find(command);
		if (it != m_command_handlers.end())
			it->second(*m_parser, xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
		else
			m_parser->onUnregisteredCommand(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	}
};

void
CommonXMLDocumentParser::trySendTag(const Tag& tag) const
{
  impl->send_tag(tag);
}

void
CommonXMLDocumentParser::activeEmittingSignals(bool flag) const
{
  impl->stop_emmit_signals = !flag;
}

class CommonXMLDocumentParser::CommandHandlersSet
{
	public:
		static void onODFOOXMLText(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_TEXT command.";
			char* content = xml_stream.content();
			if (content != NULL)
				text += content;
		}

		static void onODFOOXMLTab(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_TAB command.";
			text += "\t";
			parser.impl->send_tag(tag::Text{.text = "\t"});
		}

		static void onODFOOXMLSpace(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
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
			parser.impl->send_tag(tag::Text{.text = std::string(count, ' ')});
		}

		static void onODFOOXMLUrl(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								  const ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_URL command.";
			std::string mlink = xml_stream.attribute("href");
			parser.impl->send_tag(tag::Link{.url = mlink});
			xml_stream.levelDown();
			std::string text_link = parser.parseXmlData(xml_stream, mode, zipfile);
			text_link = formatUrl(mlink, text_link);
			xml_stream.levelUp();
			children_processed = true;
			parser.impl->send_tag(tag::CloseLink{});
		}

		static void onODFOOXMLListStyle(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
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
				parser.getListStyles()[style_code] = list_style;
			}
		}

		static void onODFOOXMLList(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			std::vector<std::string> list_vector;
			docwire_log(debug) << "ODFOOXML_LIST command.";
			++parser.getListDepth();
			std::string header;
			CommonXMLDocumentParser::ODFOOXMLListStyle list_style = CommonXMLDocumentParser::bullet;
			std::string style_name = xml_stream.attribute("style-name");
			if (parser.getListDepth() <= 10 && !style_name.empty() && parser.getListStyles().find(style_name) != parser.getListStyles().end())
				list_style = parser.getListStyles()[style_name].at(parser.getListDepth() - 1);
			std::string list_type = (list_style == CommonXMLDocumentParser::number ? "decimal" : "disc");
			parser.impl->send_tag(tag::List{.type = list_type});

			xml_stream.levelDown();
			while (xml_stream)
			{
				std::string list_element_type = xml_stream.name();
				xml_stream.levelDown();
				if (list_element_type == "list-header" && xml_stream)
				{
					header = parser.parseXmlData(xml_stream, mode, zipfile);
				}
				else if (xml_stream)
				{
					parser.impl->send_tag(tag::ListItem{});
					list_vector.push_back(parser.parseXmlData(xml_stream, mode, zipfile));
				}

				parser.impl->send_tag(tag::CloseListItem{});

				xml_stream.levelUp();
				xml_stream.next();
			}
			xml_stream.levelUp();

			if (header.length() > 0)
			{
				text += header;
				if (list_vector.size() > 0)
					text += "\n";
					parser.impl->send_tag(tag::BreakLine{});
			}
			--parser.getListDepth();
			parser.impl->send_tag(tag::CloseList{});
			if (list_style == CommonXMLDocumentParser::number)
				text += formatNumberedList(list_vector);
			else
				text += formatList(list_vector);
			children_processed = true;
		}

		static void onODFOOXMLTable(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									const ZipReader* zipfile, std::string& text,
									bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			svector cell_vector;
			std::vector<svector> row_vector;
			docwire_log(debug) << "ODFOOXML_TABLE command.";
			parser.impl->send_tag(tag::Table{});
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "table-row")
				{
					xml_stream.levelDown();
					cell_vector.clear();
					parser.impl->send_tag(tag::TableRow{});
					while (xml_stream)
					{
						if (xml_stream.name() == "table-cell")
						{
							parser.impl->send_tag(tag::TableCell{});
							xml_stream.levelDown();
							cell_vector.push_back(parser.parseXmlData(xml_stream, mode, zipfile));
							xml_stream.levelUp();
							parser.impl->send_tag(tag::CloseTableCell{});
						}
						xml_stream.next();
					}
					row_vector.push_back(cell_vector);
					xml_stream.levelUp();
					parser.impl->send_tag(tag::CloseTableRow{});
				}
				xml_stream.next();
			}
			xml_stream.levelUp();
			parser.impl->send_tag(tag::CloseTable{});
			text += formatTable(row_vector);
			children_processed = true;
		}

		static void onODFOOXMLTableRow(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									   const ZipReader* zipfile, std::string& text,
									   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODFOOXML_ROW command.";
		}

		static void onODFTableCell(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_CELL command.";
		}

		static void onODFAnnotation(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									const ZipReader* zipfile, std::string& text,
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
					parser.activeEmittingSignals(false);
					content += parser.parseXmlData(xml_stream, mode, zipfile);
					parser.activeEmittingSignals(true);
					xml_stream.levelUp();
				}
				xml_stream.next();
			}
			xml_stream.levelUp();
			parser.trySendTag(tag::Comment{.author = creator, .time = date, .comment = content});
			text += parser.formatComment(creator, date, content);
			children_processed = true;
		}

		static void onODFLineBreak(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								   const ZipReader* zipfile, std::string& text,
								   bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_LINE_BREAK command.";
			text += "\n";
			parser.impl->send_tag(tag::BreakLine{});
		}

		static void onODFHeading(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								 const ZipReader* zipfile, std::string& text,
								 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_HEADING command.";
			xml_stream.levelDown();
			text += parser.parseXmlData(xml_stream, mode, zipfile) + '\n';
			xml_stream.levelUp();
			children_processed = true;
		}

		static void onODFObject(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "ODF_OBJECT command.";
			std::string href = xml_stream.attribute("href");
			std::string content_fn = (href.substr(0, 2) == "./" ? href.substr(2) : href) + "/content.xml";
			std::string content;
			if (!zipfile->read(content_fn, &content))
			{
				docwire_log(error) << "Error reading " << content_fn;
				return;
			}
			std::string object_text;
			try
			{
				parser.extractText(content, mode, zipfile, object_text);
			}
			catch (const std::exception& e)
			{
				docwire_log(error) << "Error parsing file." << content_fn << e;
			}
			text += object_text;
		}
};

void CommonXMLDocumentParser::registerODFOOXMLCommandHandler(const std::string& xml_tag, CommandHandler handler)
{
	impl->m_command_handlers[xml_tag] = handler;
}

void CommonXMLDocumentParser::onUnregisteredCommand(XmlStream& xml_stream, XmlParseMode mode,
													const ZipReader* zipfile, std::string& text,
													bool& children_processed, std::string& level_suffix, bool first_on_level)
{
	if (impl->m_disabled_text == false && xml_stream.name() == "#text")
	{
		char* content = xml_stream.content();
		if (content != NULL)
			text += content;
		children_processed = true;
	}
}

void
CommonXMLDocumentParser::addCallback(const NewNodeCallback &callback)
{
  impl->m_on_new_node_signal.connect(callback);
}

std::string CommonXMLDocumentParser::parseXmlData(
  XmlStream& xml_stream, XmlParseMode mode, const ZipReader* zipfile) const
{
	std::string text;
	std::string level_suffix;
	bool first_on_level = true;

	while (xml_stream)
	{
		bool space_preserve_prev = impl->space_preserve;
		std::string space_attr = xml_stream.attribute("space");
		if (!space_attr.empty())
		{
			if (space_attr == "preserve")
    			impl->space_preserve = true;
			else if (space_attr == "default")
				impl->space_preserve = false;
		}
		bool children_processed;
		impl->executeCommand(xml_stream.name(), xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
		if (xml_stream && (!children_processed))
		{
			xml_stream.levelDown();
			if (xml_stream)
				text += parseXmlData(xml_stream, mode, zipfile);
			xml_stream.levelUp();
		}
		impl->space_preserve = space_preserve_prev;
		xml_stream.next();
		first_on_level = false;
	}
	if (!level_suffix.empty())
		text += level_suffix;
	return text;
}

void CommonXMLDocumentParser::extractText(const std::string& xml_contents, XmlParseMode mode, const ZipReader* zipfile, std::string& text) const
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
		throw RuntimeError("Error parsing XML contents", e);
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
		throw RuntimeError("Error while parsing ODF metadata", e);
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

size_t& CommonXMLDocumentParser::getListDepth() const
{
	return impl->m_list_depth;
}

std::map<std::string, CommonXMLDocumentParser::ListStyleVector>& CommonXMLDocumentParser::getListStyles() const
{
	return impl->m_list_styles;
}

std::map<int, CommonXMLDocumentParser::Comment>& CommonXMLDocumentParser::getComments() const
{
	return impl->m_comments;
}

std::vector<CommonXMLDocumentParser::SharedString>& CommonXMLDocumentParser::getSharedStrings() const
{
	return impl->m_shared_strings;
}

bool CommonXMLDocumentParser::disabledText() const
{
	return impl->m_disabled_text;
}

void CommonXMLDocumentParser::disableText(bool disable) const
{
	impl->m_disabled_text = disable;
}

void CommonXMLDocumentParser::setXmlOptions(int options) const
{
	impl->m_xml_options = options;
}

CommonXMLDocumentParser::CommonXMLDocumentParser()
	: impl{std::make_unique<Implementation>()}
{
		impl->m_list_depth = 0;
		impl->m_disabled_text = false;
		impl->m_xml_options = 0;
		impl->m_parser = this;

		registerODFOOXMLCommandHandler("text", &CommandHandlersSet::onODFOOXMLText);
		registerODFOOXMLCommandHandler("tab", &CommandHandlersSet::onODFOOXMLTab);
		registerODFOOXMLCommandHandler("space", &CommandHandlersSet::onODFOOXMLSpace);
		registerODFOOXMLCommandHandler("s", &CommandHandlersSet::onODFOOXMLSpace);
		registerODFOOXMLCommandHandler("a", &CommandHandlersSet::onODFOOXMLUrl);
		registerODFOOXMLCommandHandler("list-style", &CommandHandlersSet::onODFOOXMLListStyle);
		registerODFOOXMLCommandHandler("list", &CommandHandlersSet::onODFOOXMLList);
		registerODFOOXMLCommandHandler("table", &CommandHandlersSet::onODFOOXMLTable);
		registerODFOOXMLCommandHandler("table-row", &CommandHandlersSet::onODFOOXMLTableRow);
		registerODFOOXMLCommandHandler("table-cell", &CommandHandlersSet::onODFTableCell);
		registerODFOOXMLCommandHandler("annotation", &CommandHandlersSet::onODFAnnotation);
		registerODFOOXMLCommandHandler("line-break", &CommandHandlersSet::onODFLineBreak);
		registerODFOOXMLCommandHandler("h", &CommandHandlersSet::onODFHeading);
		registerODFOOXMLCommandHandler("object", &CommandHandlersSet::onODFObject);
}

CommonXMLDocumentParser::~CommonXMLDocumentParser() = default;

int CommonXMLDocumentParser::getXmlOptions() const
{
	return impl->m_xml_options;
}

} // namespace docwire
