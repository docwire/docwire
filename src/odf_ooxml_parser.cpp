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

#include "odf_ooxml_parser.h"

#include "common_xml_document_parser.h"
#include "data_source.h"
#include "document_elements.h"
#include "scoped_stack_push.h"
#include "xml_fixer.h"
#include "zip_reader.h"
#include "error_tags.h"
#include "log.h"
#include <map>
#include "misc.h"
#include <mutex>
#include <regex>
#include <stdlib.h>
#include <string.h>
#include "xml_stream.h"
#include "throw_if.h"

namespace docwire
{

using namespace std;

const int CASESENSITIVITY = 1;

static string locate_main_file(const ZipReader& zipfile)
{
	if (zipfile.exists("content.xml"))
		return "content.xml";
	if (zipfile.exists("word/document.xml"))
		return "word/document.xml";
	if (zipfile.exists("xl/workbook.xml"))
		return "xl/workbook.xml";
	if (zipfile.exists("ppt/presentation.xml"))
		return "ppt/presentation.xml";
	throw make_error("No content.xml, no word/document.xml and no ppt/presentation.xml", errors::uninterpretable_data{});
}

namespace
{

struct context
{
	const message_callbacks& emit_message;
	int last_ooxml_col_num = 0;
	int last_ooxml_row_num = 0;
};

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.oasis.opendocument.text"},
	mime_type{"application/vnd.oasis.opendocument.spreadsheet"},
	mime_type{"application/vnd.oasis.opendocument.presentation"},
	mime_type{"application/vnd.oasis.opendocument.graphics"},
	mime_type{"application/vnd.oasis.opendocument.text-template"},
	mime_type{"application/vnd.oasis.opendocument.spreadsheet-template"},
	mime_type{"application/vnd.oasis.opendocument.presentation-template"},
	mime_type{"application/vnd.oasis.opendocument.graphics-template"},
	mime_type{"application/vnd.oasis.opendocument.text-web"},
	mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
	mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
	mime_type{"application/vnd.openxmlformats-officedocument.presentationml.presentation"},
	mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
	mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
	mime_type{"application/vnd.openxmlformats-officedocument.presentationml.template"},
	mime_type{"application/vnd.openxmlformats-officedocument.presentationml.slideshow"}
};

} // anonymous namespace

template<>
struct pimpl_impl<ODFOOXMLParser> : with_pimpl_owner<ODFOOXMLParser>
{
	pimpl_impl(ODFOOXMLParser& owner) : with_pimpl_owner{owner} {}

	std::stack<context> m_context_stack;

	template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

		static void onOOXMLAttribute(XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_ATTR command.";
			children_processed = true;
		}

    void onOOXMLRow(XmlStream& xml_stream, XmlParseMode mode,
                           ZipReader* zipfile, std::string& text,
                           bool& children_processed, std::string& level_suffix, bool first_on_level)
    {
		ODFOOXMLParser& p = owner();
		p.setLastOOXMLColNum(0);
		int expected_row_num = p.lastOOXMLRowNum() + 1;
		std::string row_num_attr = xml_stream.attribute("r");
		try
		{
			int row_num = std::stoi(row_num_attr);
			if (row_num > expected_row_num)
			{
				int empty_rows_count = row_num - expected_row_num;
				for (int i = 0; i < empty_rows_count; i++)
				{
					emit_message(document::TableRow{});
					emit_message(document::CloseTableRow{});
				}
			}
			p.setLastOOXMLRowNum(row_num);
		}
		catch (const std::invalid_argument& ex)
		{
			// we accept when row id attribute is incorrect or missing
			p.setLastOOXMLRowNum(expected_row_num);
		}
      emit_message(document::TableRow{});
      xml_stream.levelDown();
      text += owner().parseXmlData(xml_stream, mode, zipfile);
      xml_stream.levelUp();
      emit_message(document::CloseTableRow{});
    }

	void onOOXMLSheetData(XmlStream& xml_stream, XmlParseMode mode,
                         ZipReader* zipfile, std::string& text,
                         bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
	  ODFOOXMLParser& p = owner();
	  p.setLastOOXMLRowNum(0);
    emit_message(document::Table{});
    xml_stream.levelDown();
    text += p.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    emit_message(document::CloseTable{});
  }

	void onOOXMLCell(XmlStream& xml_stream, XmlParseMode mode,
								ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_CELL command.";
			if (!first_on_level)
      {
				text += "\t";
      }
			ODFOOXMLParser& p = owner();
			int expected_col_num = p.lastOOXMLColNum() + 1;
			std::string cell_addr_attr = xml_stream.attribute("r");
			bool matched;
			std::smatch match;
			static std::mutex cell_addr_rgx_mutex;
			{
				std::lock_guard<std::mutex> cell_addr_rgx_mutex_lock(cell_addr_rgx_mutex);
				static const std::regex cell_addr_rgx("([A-Z]+)([0-9]+)");
				matched = std::regex_search(cell_addr_attr, match, cell_addr_rgx);
			}
			if (matched && match.size() == 3)
			{
				std::string col_addr = match[1].str();
				std::string row_addr = match[2].str();
				int col_num = 0;
				for (char ch: col_addr)
					col_num = col_num * 26 + (ch - 'A') + 1;
				int row_num = std::stoi(row_addr);
				if (col_num > expected_col_num)
				{
					int empty_cols_count = col_num - expected_col_num;
					for (int i = 0; i < empty_cols_count; i++)
					{
						emit_message(document::TableCell{});
						emit_message(document::CloseTableCell{});
					}
				}
				p.setLastOOXMLColNum(col_num);
			}
			else
			{
				// we accept when cell address attribute is incorrect or missing
				p.setLastOOXMLColNum(expected_col_num);
			}
			emit_message(document::TableCell{});
			if (xml_stream.attribute("t") == "s")
			{
				xml_stream.levelDown();
        		p.activeEmittingSignals(false);
				int shared_string_index = str_to_int(p.parseXmlData(xml_stream, mode, zipfile));
        		owner().activeEmittingSignals(true);
				xml_stream.levelUp();
				if (shared_string_index < p.getSharedStrings().size())
				{
					text += p.getSharedStrings()[shared_string_index].m_text;
					emit_message(document::Text{.text = p.getSharedStrings()[shared_string_index].m_text});
				}
			}
			else
			{
				xml_stream.levelDown();
    			text += p.parseXmlData(xml_stream, mode, zipfile);
				xml_stream.levelUp();
			}
			emit_message(document::CloseTableCell{});
			level_suffix = "\n";
			children_processed = true;
		}

		static void onOOXMLHeaderFooter(XmlStream& xml_stream, XmlParseMode mode,
										const ZipReader* zipfile, std::string& text,
										bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_HEADERFOOTER command.";
			// Ignore headers and footers. They can contain some commands like font settings that can mess up output.
			// warning TODO: Better headers and footers support
			children_processed = true;
		}

	void onOOXMLCommentReference(XmlStream& xml_stream, XmlParseMode mode,
											const ZipReader* zipfile, std::string& text,
											bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_COMMENTREFERENCE command.";
			int comment_id = str_to_int(xml_stream.attribute("id"));
			if (owner().getComments().count(comment_id))
			{
				const ODFOOXMLParser::Comment& c = owner().getComments()[comment_id];
				text += owner().formatComment(c.m_author, c.m_time, c.m_text);
				emit_message(document::Comment{.author = c.m_author, .time = c.m_time, .comment = c.m_text});
			}
			else
				emit_message(make_error_ptr("Comment not found, skipping", comment_id));
		}

	void onOOXMLHyperlink(XmlStream& xml_stream, XmlParseMode mode,
								  ZipReader* zipfile, std::string& text,
								  bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_HYPERLINK command.";
			std::string rid = xml_stream.attribute("id");
			if (owner().getRelationships().count(rid))
			{
				const ODFOOXMLParser::Relationship& r = owner().getRelationships()[rid];
				emit_message(document::Link{.url = r.m_target});
				xml_stream.levelDown();
				std::string link_text = owner().parseXmlData(xml_stream, mode, zipfile);
				xml_stream.levelUp();
				children_processed = true;
				text += formatUrl(r.m_target, link_text);
				emit_message(document::CloseLink{});
			}
			else
				emit_message(make_error_ptr("Relationship not found, skipping", rid));
		}

		static void onOOXMLInstrtext(XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_INSTRTEXT command.";
			children_processed = true;
		}

		static void onOOXMLTableStyleId(XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_TABLESTYLEID command.";
			// Ignore style identifier that is embedded as text inside this tag not to treat it as a document text.
			children_processed = true;
		}

  void
  onOOXMLStyle(XmlStream& xml_stream, XmlParseMode mode,
                               ZipReader* zipfile, std::string& text,
                               bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
    xml_stream.levelDown();
    owner().activeEmittingSignals(false);
    text += owner().parseXmlData(xml_stream, mode, zipfile);
		xml_stream.levelUp();
    children_processed = true;
    owner().activeEmittingSignals(true);
  }

	void onOOXMLBreak(XmlStream& xml_stream, XmlParseMode mode,
								const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level) const
	{
		docwire_log(debug) << "OOXML_BREAK command.";
		text += "\n";

		emit_message(document::BreakLine{});
	}

	void assertODFFileIsNotEncrypted(ZipReader& zipfile) const
	{
		std::string content;
		throw_if (zipfile.exists("META-INF/manifest.xml")
			&& zipfile.read("META-INF/manifest.xml", &content)
			&& content.find("manifest:encryption-data") != std::string::npos,
			errors::file_encrypted{});
	}

	void readOOXMLComments(ZipReader& zipfile, XmlParseMode mode)
	{
		std::string content;
		throw_if (!zipfile.read("word/comments.xml", &content), "Error reading word/comments.xml");
		std::string xml;
		if (mode == FIX_XML)
		{
			XmlFixer xml_fixer;
			xml = xml_fixer.fix(content);
		}
		else
			xml = content;
		try
		{
			XmlStream xml_stream(xml, owner().no_blanks());
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "comment")
				{
					int id = str_to_int(xml_stream.attribute("id"));
					std::string author = xml_stream.attribute("author");
					std::string date = xml_stream.attribute("date");
					xml_stream.levelDown();
					owner().activeEmittingSignals(false);
					std::string text = owner().parseXmlData(xml_stream, mode, &zipfile);
					owner().activeEmittingSignals(true);
					xml_stream.levelUp();
					CommonXMLDocumentParser::Comment c(author, date, text);
					owner().getComments()[id] = c;
				}
				xml_stream.next();
			}
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error("Error parsing word/comments.xml."));
		}
	}

	void readOOXMLRelationships(ZipReader& zipfile, XmlParseMode mode)
	{
		std::string content;
		throw_if (!zipfile.read("word/_rels/document.xml.rels", &content), "Error reading word/_rels/document.xml.rels");
		std::string xml;
		if (mode == FIX_XML)
		{
			XmlFixer xml_fixer;
			xml = xml_fixer.fix(content);
		}
		else
			xml = content;
		try
		{
			XmlStream xml_stream(xml, owner().no_blanks());
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "Relationship")
				{
					std::string id = xml_stream.attribute("Id");
					std::string target = xml_stream.attribute("Target");
					CommonXMLDocumentParser::Relationship r{target};
					owner().getRelationships()[id] = r;
				}
				xml_stream.next();
			}
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error("Error parsing word/_rels/document.xml.rels."));
		}
	}

	void readStyles(ZipReader& zipfile, XmlParseMode mode, const message_callbacks& emit_message)
	{
		std::string content;
		throw_if (!zipfile.read("styles.xml", &content), "Error reading styles.xml");
		std::string xml;
		if (mode == FIX_XML)
		{
			XmlFixer xml_fixer;
			xml = xml_fixer.fix(content);
		}
		else
			xml = content;
		try
		{
			XmlStream xml_stream(xml, owner().no_blanks());
			owner().parseXmlData(xml_stream, mode, &zipfile);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error("Error parsing styles.xml."));
		}
	}
};

ODFOOXMLParser::ODFOOXMLParser()
{
	registerODFOOXMLCommandHandler("attrName", [impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLAttribute(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("c", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLCell(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
    registerODFOOXMLCommandHandler("row", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLRow(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
    registerODFOOXMLCommandHandler("sheetData", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLSheetData(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("headerFooter", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLHeaderFooter(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("commentReference", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLCommentReference(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("hyperlink", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLHyperlink(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("br", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, const ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLBreak(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    });
	registerODFOOXMLCommandHandler("document-styles", [&impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLStyle(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    });
	registerODFOOXMLCommandHandler("instrText", [impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLInstrtext(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
	registerODFOOXMLCommandHandler("tableStyleId", [impl=impl()](XmlStream& xml_stream, XmlParseMode mode, ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
	{
		impl.onOOXMLTableStyleId(xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
	});
}

int ODFOOXMLParser::lastOOXMLRowNum()
{
	return impl().m_context_stack.top().last_ooxml_row_num;
}

void ODFOOXMLParser::setLastOOXMLRowNum(int r)
{
	impl().m_context_stack.top().last_ooxml_row_num = r;
}

int ODFOOXMLParser::lastOOXMLColNum()
{
	return impl().m_context_stack.top().last_ooxml_col_num;
}

void ODFOOXMLParser::setLastOOXMLColNum(int c)
{
	impl().m_context_stack.top().last_ooxml_col_num = c;
}

void ODFOOXMLParser::parse(const data_source& data, XmlParseMode mode, const message_callbacks& emit_message)
{
	ZipReader zipfile{data};
	try
	{
		zipfile.open();
		string main_file_name;
		try
		{
			main_file_name = locate_main_file(zipfile);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(make_error("Invalid file structure"));
		}
		emit_message(document::Document{.metadata=[this, &zipfile](){ return metaData(zipfile);}});
	//according to the ODF specification, we must skip blank nodes. Otherwise output may be messed up.
	if (main_file_name == "content.xml")
	{
		impl().assertODFFileIsNotEncrypted(zipfile);
		set_no_blanks(XmlStream::no_blanks{true});
	}
	if (zipfile.exists("word/comments.xml"))
	{
		try
		{
			impl().readOOXMLComments(zipfile, mode);
		}
		catch (const std::exception&)
		{
			emit_message(make_nested_ptr(std::current_exception(), make_error("Error parsing comments.")));
		}
	}
	if (zipfile.exists("word/_rels/document.xml.rels"))
	{
		try
		{
			impl().readOOXMLRelationships(zipfile, mode);
		}
		catch (const std::exception&)
		{
			emit_message(make_nested_ptr(std::current_exception(), make_error("Error parsing relationships.")));
		}
	}
	if (zipfile.exists("styles.xml"))
		impl().readStyles(zipfile, mode, emit_message);
	string content;
	if (main_file_name == "ppt/presentation.xml")
	{
		throw_if (!zipfile.loadDirectory());
		for (int i = 1; zipfile.read("ppt/slides/slide" + int_to_str(i) + ".xml", &content) && i < 2500; i++)
		{
			try
			{
				std::string text;
				extractText(content, mode, &zipfile, text);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(errors::impl{std::make_pair("file_name", "ppt/slides/slide" + int_to_str(i) + ".xml")});
			}
		}
	}
	else if (main_file_name == "xl/workbook.xml")
	{
		if (!zipfile.read("xl/sharedStrings.xml", &content))
		{
			//file may not exist, but this is not reason to report an error.
			docwire_log(debug) << "xl/sharedStrings.xml does not exist";
		}
		else
		{
			std::string xml;
			if (mode == FIX_XML)
			{
				XmlFixer xml_fixer;
				xml = xml_fixer.fix(content);
			}
			else if (mode == PARSE_XML)
				xml = content;
			else
				throw_if(mode == STRIP_XML, "Stripping XML is not possible for xlsx files", errors::program_logic{});
			try
			{
				XmlStream xml_stream(xml, no_blanks());
				xml_stream.levelDown();
				while (xml_stream)
				{
					if (xml_stream.name() == "si")
					{
						xml_stream.levelDown();
						SharedString shared_string;
            activeEmittingSignals(false);
						shared_string.m_text = parseXmlData(xml_stream, mode, &zipfile);
            activeEmittingSignals(true);
						getSharedStrings().push_back(shared_string);
						xml_stream.levelUp();
					}
					xml_stream.next();
				}
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(errors::impl{std::make_pair("file_name", "xl/sharedStrings.xml")});
			}
		}
		for (int i = 1; zipfile.read("xl/worksheets/sheet" + int_to_str(i) + ".xml", &content); i++)
		{
			try
			{
				std::string text;
				extractText(content, mode, &zipfile, text);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(errors::impl{make_pair("file_name", "xl/worksheets/sheet" + int_to_str(i) + ".xml")});
			}
		}
	}
	else
	{
		throw_if(!zipfile.read(main_file_name, &content), "Error reading XML file from ZIP file", main_file_name);
		try
		{
			std::string text;
			extractText(content, mode, &zipfile, text);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error(main_file_name));
		}
	}
	emit_message(document::CloseDocument{});
	}
	catch (const std::exception& e)
	{
		throw_if (is_encrypted_with_ms_offcrypto(data), errors::file_encrypted{}, "Microsoft Office Document Cryptography");
		throw;
	}
}

attributes::Metadata ODFOOXMLParser::metaData(ZipReader& zipfile) const
{
	docwire_log(debug) << "Extracting metadata.";
	attributes::Metadata meta;
	if (zipfile.exists("meta.xml"))
	{
		impl().assertODFFileIsNotEncrypted(zipfile);
		std::string meta_xml;
		throw_if (!zipfile.read("meta.xml", &meta_xml), "Error reading XML file from ZIP file", std::make_pair("file_name", "meta.xml" ));
		try
		{
			parseODFMetadata(meta_xml, meta);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(errors::impl{std::make_pair("file_name", "meta.xml")});
		}
	}
	else if (zipfile.exists("docProps/core.xml"))
	{
		std::string core_xml;
		throw_if (!zipfile.read("docProps/core.xml", &core_xml), "Error reading XML file from ZIP file", std::make_pair("file_name", "docProps/core.xml"));
		try
		{
			XmlStream xml_stream(core_xml, no_blanks());
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "creator")
					meta.author = xml_stream.stringValue();
				if (xml_stream.name() == "created")
				{
					tm creation_date;
					string_to_date(xml_stream.stringValue(), creation_date);
					meta.creation_date = creation_date;
				}
				if (xml_stream.name() == "lastModifiedBy")
					meta.last_modified_by = xml_stream.stringValue();
				if (xml_stream.name() == "modified")
				{
					tm last_modification_date;
					string_to_date(xml_stream.stringValue(), last_modification_date);
					meta.last_modification_date = last_modification_date;
				}
				xml_stream.next();
			}
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(errors::impl{std::make_pair("file_name", "docProps/core.xml")});
		}

		std::string app_xml;
		throw_if (!zipfile.read("docProps/app.xml", &app_xml), "Error reading XML file from ZIP file", std::make_pair("file_name", "docProps/app.xml"));
		try
		{
			XmlStream app_stream(app_xml, no_blanks());
			app_stream.levelDown();
			while (app_stream)
			{
				if (app_stream.name() == "Pages")
					meta.page_count = str_to_int(app_stream.stringValue());
				if (app_stream.name() == "Words")
					meta.word_count = str_to_int(app_stream.stringValue());
				app_stream.next();
			}
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(errors::impl{std::make_pair("file_name", "docProps/app.xml")});
		}
	}
	if (!meta.page_count)
	{
		// If we are processing PPT use slide count as page count
		if (zipfile.exists("ppt/presentation.xml"))
		{
			int page_count = 0;
			for (int i = 1; zipfile.exists("ppt/slides/slide" + int_to_str(i) + ".xml"); i++)
				page_count++;
			meta.page_count = page_count;
		}
		else if (zipfile.exists("content.xml"))
		{
			// If we are processing ODP use slide count as page count
			// If we are processing ODG extract page count the same way
			std::string content;
			if (zipfile.read("content.xml", &content) &&
				(content.find("<office:presentation") != std::string::npos ||
				content.find("<office:drawing") != std::string::npos))
			{
				int page_count = 0;
				std::string page_str = "<draw:page ";
				for (size_t pos = content.find(page_str); pos != std::string::npos;
						pos = content.find(page_str, pos + page_str.length()))
					page_count++;
				meta.page_count = page_count;
			}
		}
	}
	return meta;
}

void
ODFOOXMLParser::parse(const data_source& data, const message_callbacks& emit_message)
{
	docwire_log(debug) << "Using ODF/OOXML parser.";
	CommonXMLDocumentParser::scoped_context_stack_push base_context_guard{*this, emit_message};
	scoped::stack_push<context> context_guard{impl().m_context_stack, context{emit_message}};
	parse(data, XmlParseMode::PARSE_XML, emit_message);
}

continuation ODFOOXMLParser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
	{
		return emit_message(std::move(msg));
	}

	parse(data, emit_message); // Call the existing public parse method
	return continuation::proceed;
}

} // namespace docwire
