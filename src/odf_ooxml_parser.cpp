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

#include "xml_fixer.h"
#include "zip_reader.h"
#include "exception.h"
#include <fstream>
#include <iostream>
#include <libxml2/libxml/xmlreader.h>
#include "log.h"
#include <map>
#include "misc.h"
#include <mutex>
#include <regex>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <boost/signals2.hpp>
#include "xml_stream.h"
#include "thread_safe_ole_storage.h"
#include "thread_safe_ole_stream_reader.h"

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
	docwire_log(error) << "Error - no content.xml, no word/document.xml and no ppt/presentation.xml";
	return "";
}

class ODFOOXMLParser::CommandHandlersSet
{
	public:
		static void onOOXMLAttribute(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_ATTR command.";
			children_processed = true;
		}

    static void onOOXMLRow(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                           const ZipReader* zipfile, std::string& text,
                           bool& children_processed, std::string& level_suffix, bool first_on_level)
    {
		ODFOOXMLParser& p = (ODFOOXMLParser&)parser;
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
					parser.trySendTag(tag::TableRow{});
					parser.trySendTag(tag::CloseTableRow{});
				}
			}
			p.setLastOOXMLRowNum(row_num);
		}
		catch (std::invalid_argument const& ex)
		{
			// we accept when row id attribute is incorrect or missing
			p.setLastOOXMLRowNum(expected_row_num);
		}
      parser.trySendTag(tag::TableRow{});
      xml_stream.levelDown();
      text += parser.parseXmlData(xml_stream, mode, zipfile);
      xml_stream.levelUp();
      parser.trySendTag(tag::CloseTableRow{});
    }

  static void onOOXMLSheetData(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                         const ZipReader* zipfile, std::string& text,
                         bool& children_processed, std::string& level_suffix, bool first_on_level)
  {
	  ODFOOXMLParser& p = (ODFOOXMLParser&)parser;
	  p.setLastOOXMLRowNum(0);
    parser.trySendTag(tag::Table{});
    xml_stream.levelDown();
    text += parser.parseXmlData(xml_stream, mode, zipfile);
    xml_stream.levelUp();
    parser.trySendTag(tag::CloseTable{});
  }

		static void onOOXMLCell(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
								const ZipReader* zipfile, std::string& text,
								bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_CELL command.";
			if (!first_on_level)
      {
				text += "\t";
      }
			ODFOOXMLParser& p = (ODFOOXMLParser&)parser;
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
						parser.trySendTag(tag::TableCell{});
						parser.trySendTag(tag::CloseTableCell{});
					}
				}
				p.setLastOOXMLColNum(col_num);
			}
			else
			{
				// we accept when cell address attribute is incorrect or missing
				p.setLastOOXMLRowNum(expected_col_num);
			}
      parser.trySendTag(tag::TableCell{});
			if (xml_stream.attribute("t") == "s")
			{
				xml_stream.levelDown();
        parser.activeEmittingSignals(false);
				int shared_string_index = str_to_int(parser.parseXmlData(xml_stream, mode, zipfile));
        parser.activeEmittingSignals(true);
				xml_stream.levelUp();
				if (shared_string_index < parser.getSharedStrings().size())
				{
					text += parser.getSharedStrings()[shared_string_index].m_text;
					parser.trySendTag(tag::Text{.text = parser.getSharedStrings()[shared_string_index].m_text});
				}
			}
			else
			{
				xml_stream.levelDown();
        text += parser.parseXmlData(xml_stream, mode, zipfile);
				xml_stream.levelUp();
			}
			parser.trySendTag(tag::CloseTableCell{});
			level_suffix = "\n";
			children_processed = true;
		}

		static void onOOXMLHeaderFooter(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
										const ZipReader* zipfile, std::string& text,
										bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_HEADERFOOTER command.";
			// Ignore headers and footers. They can contain some commands like font settings that can mess up output.
			// warning TODO: Better headers and footers support
			children_processed = true;
		}

		static void onOOXMLCommentReference(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
											const ZipReader* zipfile, std::string& text,
											bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_COMMENTREFERENCE command.";
			int comment_id = str_to_int(xml_stream.attribute("id"));
			if (parser.getComments().count(comment_id))
			{
				const Comment& c = parser.getComments()[comment_id];
				text += parser.formatComment(c.m_author, c.m_time, c.m_text);
				parser.trySendTag(tag::Comment{.author = c.m_author, .time = c.m_time, .comment = c.m_text});
			}
			else
				docwire_log(warning) << "Comment with id " << comment_id << " not found, skipping.";
		}

		static void onOOXMLInstrtext(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_INSTRTEXT command.";
			children_processed = true;
		}

		static void onOOXMLTableStyleId(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
									 const ZipReader* zipfile, std::string& text,
									 bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			docwire_log(debug) << "OOXML_TABLESTYLEID command.";
			// Ignore style identifier that is embedded as text inside this tag not to treat it as a document text.
			children_processed = true;
		}
};

struct ODFOOXMLParser::ExtendedImplementation
{
	int last_ooxml_col_num = 0;
	int last_ooxml_row_num = 0;
	const ODFOOXMLParser* m_interf;
	boost::signals2::signal<void(Info &info)> m_on_new_node_signal;

	ExtendedImplementation(const ODFOOXMLParser* interf)
		: m_interf(interf)
	{}

  void
  onOOXMLStyle(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                               const ZipReader* zipfile, std::string& text,
                               bool& children_processed, std::string& level_suffix, bool first_on_level) const
  {
    xml_stream.levelDown();
    parser.activeEmittingSignals(false);
    text += parser.parseXmlData(xml_stream, mode, zipfile);
		xml_stream.levelUp();
    children_processed = true;
    parser.activeEmittingSignals(true);
  }

	void assertODFFileIsNotEncrypted(const ZipReader& zipfile)
	{
		std::string content;
		if (zipfile.exists("META-INF/manifest.xml")
			&& zipfile.read("META-INF/manifest.xml", &content)
			&& content.find("manifest:encryption-data") != std::string::npos)
			throw EncryptedFileException("Specified ODF file is encrypted");
	}

	bool readOOXMLComments(const ZipReader& zipfile, XmlParseMode mode)
	{
		std::string content;
		if (!zipfile.read("word/comments.xml", &content))
		{
			docwire_log(error) << "Error reading word/comments.xml";
			return false;
		}
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
			XmlStream xml_stream(xml, m_interf->getXmlOptions());
			xml_stream.levelDown();
			while (xml_stream)
			{
				if (xml_stream.name() == "comment")
				{
					int id = str_to_int(xml_stream.attribute("id"));
					std::string author = xml_stream.attribute("author");
					std::string date = xml_stream.attribute("date");
					xml_stream.levelDown();
					m_interf->activeEmittingSignals(false);
					std::string text = m_interf->parseXmlData(xml_stream, mode, &zipfile);
					m_interf->activeEmittingSignals(true);
					xml_stream.levelUp();
					CommonXMLDocumentParser::Comment c(author, date, text);
					m_interf->getComments()[id] = c;
				}
				xml_stream.next();
			}
		}
		catch (const std::exception& e)
		{
			docwire_log(error) << "Error parsing word/comments.xml. Error message: " << e.what();
			return false;
		}
		return true;
	}

	void readStyles(const ZipReader& zipfile, XmlParseMode mode)
	{
		std::string content;
		if (!zipfile.read("styles.xml", &content))
		{
			docwire_log(error) << "Error reading styles.xml";
			return;
		}
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
			XmlStream xml_stream(xml, m_interf->getXmlOptions());
			m_interf->parseXmlData(xml_stream, mode, &zipfile);
		}
		catch (const std::exception& e)
		{
			docwire_log(error) << "Error parsing styles.xml. Error message: " << e.what();
			return;
		}
	}
};

ODFOOXMLParser::ODFOOXMLParser()
	: extended_impl{std::make_unique<ExtendedImplementation>(this)}
{
		registerODFOOXMLCommandHandler("attrName", &CommandHandlersSet::onOOXMLAttribute);
		registerODFOOXMLCommandHandler("c", &CommandHandlersSet::onOOXMLCell);
    registerODFOOXMLCommandHandler("row", &CommandHandlersSet::onOOXMLRow);
    registerODFOOXMLCommandHandler("sheetData", &CommandHandlersSet::onOOXMLSheetData);
		registerODFOOXMLCommandHandler("headerFooter", &CommandHandlersSet::onOOXMLHeaderFooter);
		registerODFOOXMLCommandHandler("commentReference", &CommandHandlersSet::onOOXMLCommentReference);
		registerODFOOXMLCommandHandler("br", [this](CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode, const ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			onOOXMLBreak(parser, xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    	});
		registerODFOOXMLCommandHandler("document-styles", [this](CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode, const ZipReader* zipfile, std::string& text, bool& children_processed, std::string& level_suffix, bool first_on_level)
		{
			extended_impl->onOOXMLStyle(parser, xml_stream, mode, zipfile, text, children_processed, level_suffix, first_on_level);
    	});
		registerODFOOXMLCommandHandler("instrText", &CommandHandlersSet::onOOXMLInstrtext);
		registerODFOOXMLCommandHandler("tableStyleId", &CommandHandlersSet::onOOXMLTableStyleId);
}

ODFOOXMLParser::~ODFOOXMLParser() = default;

Parser&
ODFOOXMLParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
	return *this;
}

int ODFOOXMLParser::lastOOXMLRowNum()
{
	return extended_impl->last_ooxml_row_num;
}

void ODFOOXMLParser::setLastOOXMLRowNum(int r)
{
	extended_impl->last_ooxml_row_num = r;
}

int ODFOOXMLParser::lastOOXMLColNum()
{
	return extended_impl->last_ooxml_col_num;
}

void ODFOOXMLParser::setLastOOXMLColNum(int c)
{
	extended_impl->last_ooxml_col_num = c;
}

void
ODFOOXMLParser::onOOXMLBreak(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                             const ZipReader* zipfile, std::string& text,
                             bool& children_processed, std::string& level_suffix, bool first_on_level) const
{
	docwire_log(debug) << "OOXML_BREAK command.";
	text += "\n";

	parser.trySendTag(tag::BreakLine{});
}

bool ODFOOXMLParser::understands(const data_source& data) const
{
	try
	{
		ZipReader zipfile{data};
		zipfile.open();
		string main_file_name = locate_main_file(zipfile);
		if (main_file_name == "")
			return false;
		string contents;
		if (!zipfile.read(main_file_name, &contents, 1))
			return false;
	}
	catch (const std::exception&)
	{
		if (is_encrypted_with_ms_offcrypto(data))
		{
			throw EncryptedFileException("File is encrypted according to the Microsoft Office Document Cryptography Specification. Exact file format cannot be determined");
		}
		return false;
	}
	return true;
}

void ODFOOXMLParser::parse(const data_source& data, XmlParseMode mode) const
{
	ZipReader zipfile{data};
	try
	{
		zipfile.open();
	string main_file_name = locate_main_file(zipfile);
	if (main_file_name == "")
		throw RuntimeError("Could not locate main file inside zip archive");
		trySendTag(tag::Document{.metadata=[this, &zipfile](){ return metaData(zipfile);}});
	//according to the ODF specification, we must skip blank nodes. Otherwise output may be messed up.
	if (main_file_name == "content.xml")
	{
		extended_impl->assertODFFileIsNotEncrypted(zipfile);
		setXmlOptions(XML_PARSE_NOBLANKS);
	}
	if (zipfile.exists("word/comments.xml") && !extended_impl->readOOXMLComments(zipfile, mode))
		docwire_log(error) << "Error parsing comments.";
	if (zipfile.exists("styles.xml"))
		extended_impl->readStyles(zipfile, mode);
	string content;
	if (main_file_name == "ppt/presentation.xml")
	{
		if (!zipfile.loadDirectory())
			throw RuntimeError("Error loading zip directory from zip archive");
		for (int i = 1; zipfile.read("ppt/slides/slide" + int_to_str(i) + ".xml", &content) && i < 2500; i++)
		{
			try
			{
				std::string text;
				extractText(content, mode, &zipfile, text);
			}
			catch (const std::exception& e)
			{
				throw RuntimeError("Error while parsing file ppt/slides/slide" + int_to_str(i) + ".xml", e);
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
				throw RuntimeError("XML stripping not possible for xlsx format");
			try
			{
				XmlStream xml_stream(xml, getXmlOptions());
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
				throw RuntimeError("Error parsing xl/sharedStrings.xml", e);
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
				throw RuntimeError("Error while parsing file xl/worksheets/sheet" + int_to_str(i) + ".xml", e);
			}
		}
	}
	else
	{
		if (!zipfile.read(main_file_name, &content))
			throw RuntimeError("Error reading xml contents of " + main_file_name);
		try
		{
			std::string text;
			extractText(content, mode, &zipfile, text);
		}
		catch (const std::exception& e)
		{
			throw RuntimeError("Error parsing xml contents of " + main_file_name, e);
		}
	}
	trySendTag(tag::CloseDocument{});
	}
	catch (const std::exception& e)
	{
		if (is_encrypted_with_ms_offcrypto(data))
			throw EncryptedFileException("File is encrypted according to the Microsoft Office Document Cryptography Specification. Exact file format cannot be determined");
		throw;
	}
}

attributes::Metadata ODFOOXMLParser::metaData(ZipReader& zipfile) const
{
	docwire_log(debug) << "Extracting metadata.";
	attributes::Metadata meta;
	if (zipfile.exists("meta.xml"))
	{
		extended_impl->assertODFFileIsNotEncrypted(zipfile);
		std::string meta_xml;
		if (!zipfile.read("meta.xml", &meta_xml))
		{
			throw RuntimeError("Error reading meta.xml");
		}
		try
		{
			parseODFMetadata(meta_xml, meta);
		}
		catch (const std::exception& e)
		{
			throw RuntimeError("Error parsing meta.xml", e);
		}
	}
	else if (zipfile.exists("docProps/core.xml"))
	{
		std::string core_xml;
		if (!zipfile.read("docProps/core.xml", &core_xml))
		{
			throw RuntimeError("Error reading docProps/core.xml");
		}
		try
		{
			XmlStream xml_stream(core_xml, getXmlOptions());
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
			throw RuntimeError("Error parsing docProps/core.xml", e);
		}

		std::string app_xml;
		if (!zipfile.read("docProps/app.xml", &app_xml))
			throw RuntimeError("Error reading docProps/app.xml");
		try
		{
			XmlStream app_stream(app_xml, getXmlOptions());
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
			throw RuntimeError("Error parsing docProps/app.xml", e);
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
ODFOOXMLParser::parse(const data_source& data) const
{
	docwire_log(debug) << "Using ODF/OOXML parser.";
	*extended_impl = ExtendedImplementation{this};
	parse(data, XmlParseMode::PARSE_XML);
}

Parser& ODFOOXMLParser::addOnNewNodeCallback(NewNodeCallback callback)
{
	CommonXMLDocumentParser::addCallback(callback);
	return *this;
}

} // namespace docwire
