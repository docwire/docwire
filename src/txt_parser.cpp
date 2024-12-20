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

#include "txt_parser.h"

#include "charsetdetect.h"
#include "htmlcxx/html/CharsetConverter.h"
#include <boost/signals2.hpp>
#include "log.h"
#include "make_error.h"
#include <string.h>

namespace docwire
{

TXTParser::TXTParser()
{
}

namespace
{

std::string sequences_of_printable_characters(const std::string& text, size_t min_seq_len = 4, char seq_delim = '\n')
{
	std::string result;
	result.reserve(text.length());
	std::string printable_field;
	printable_field.reserve(text.length());
	size_t printable_count = 0;
	size_t non_printable_count = 0;
	for (auto const& ch: text)
	{
		if (std::isprint(ch))
		{
			printable_field += ch;
			printable_count++;
			non_printable_count = 0;
		}
		else
		{
			if (printable_count >= min_seq_len)
			{
				result += printable_field;
				if (non_printable_count == 0)
					result += seq_delim;
			}
			printable_field.clear();
			printable_count = 0;
			non_printable_count++;
		}
	}
	result += printable_field;
	return result;
}

} // anonymous namespace

bool TXTParser::understands(const data_source& data) const
{
	return true;
}

void TXTParser::parse(const data_source& data) const
{
	docwire_log(debug) << "Using TXT parser.";
	std::string text;
	csd_t charset_detector = NULL;
	htmlcxx::CharsetConverter* converter = NULL;
	try
	{
		std::string encoding;
		std::string content = data.string();
		charset_detector = csd_open();
		if (charset_detector == (csd_t)-1)
		{
			charset_detector = NULL;
			sendTag(make_error_ptr("Could not create charset detector"));
			encoding = "UTF-8";
		}
		else
		{
			csd_consider(charset_detector, content.c_str(), content.length());
			const char* res = csd_close(charset_detector);
			charset_detector = NULL;
			if (res != NULL)
			{
				encoding = std::string(res);
				docwire_log(debug) << "Estimated encoding: " + encoding;
			}
			else
			{
				encoding = "ASCII";
				docwire_log(debug) << "Could not detect encoding. Document is assumed to be encoded in ASCII";
				docwire_log(debug) << "But it can be also binary. Sequences of printable characters will be extracted.";
				content = sequences_of_printable_characters(content);
			}
		}
		if (encoding != "utf-8" && encoding != "UTF-8")
		{
			try
			{
				converter = new htmlcxx::CharsetConverter(encoding, "UTF-8");
			}
			catch (htmlcxx::CharsetConverter::Exception& ex)
			{
				sendTag(make_nested_ptr(ex, make_error("Cannot convert text to UTF-8", encoding)));
				if (converter)
					delete converter;
				converter = NULL;
			}
		}
		if (converter)
		{
			text = converter->convert(content);
			delete converter;
			converter = NULL;
		}
		else
			text = content;
	}
	catch (const std::exception& e)
	{
		if (converter)
			delete converter;
		converter = NULL;
		if (charset_detector)
			csd_close(charset_detector);
		charset_detector = NULL;
		std::throw_with_nested(make_error("Error converting text to UTF-8"));
	}
	bool parse_paragraphs = m_parameters.getParameterValue<bool>("TXTParser::parse_paragraphs").value_or(true);
	bool parse_lines = m_parameters.getParameterValue<bool>("TXTParser::parse_lines").value_or(true);
	sendTag(tag::Document{});
	if (parse_lines || parse_paragraphs)
	{
		std::string::size_type curr_pos = 0;
		enum { outside_paragraph, empty_paragraph, filled_paragraph } paragraph_state = outside_paragraph;
		std::string last_eol = "";
		for (;;)
		{
			std::string::size_type eol_pos = text.find_first_of("\r\n", curr_pos);
			std::string eol = (eol_pos == std::string::npos ? std::string{""} : text.substr(eol_pos, 1));
			if (eol == "\r" && eol_pos + 1 < text.size() && text[eol_pos + 1] == '\n')
				eol += '\n';
			std::string line = text.substr(curr_pos, eol_pos - curr_pos);
			if (parse_paragraphs)
			{
				if (paragraph_state == outside_paragraph)
				{
					sendTag(tag::Paragraph{});
					paragraph_state = empty_paragraph;
				}
				if (line.empty())
				{
					sendTag(tag::CloseParagraph{});
					paragraph_state = outside_paragraph;
				}
				else
				{
					if (paragraph_state == filled_paragraph)
					{
						if (parse_lines)
							sendTag(tag::BreakLine{});
						else
							sendTag(tag::Text{.text = last_eol});
					}
					sendTag(tag::Text{.text = line});
					paragraph_state = filled_paragraph;
				}
			}
			else
			{
				if (!line.empty())
					sendTag(tag::Text{.text = line});
				if (!eol.empty())
				{
					if (parse_lines)
						sendTag(tag::BreakLine{});
					else
						sendTag(tag::Text{.text = eol});
				}
			}
			if (eol.empty())
				break;
			curr_pos = eol_pos + eol.size();
			last_eol = eol;
		}
		if (parse_paragraphs && paragraph_state != outside_paragraph)
			sendTag(tag::CloseParagraph{});
	}
	else
		sendTag(tag::Text{.text = text});
	sendTag(tag::CloseDocument{});
}

std::vector<file_extension> TXTParser::getExtensions()
{
	return
	{
		file_extension{".asm"}, // Assembler source code
		file_extension{".asp"}, // Active Server Page script page
		file_extension{".aspx"}, // Active Server Page Extended ASP.NET script
		file_extension{".bas"}, // Basic source code
		file_extension{".bat"}, // Batch file (script)
		file_extension{".c"}, // C source code
		file_extension{".cc"}, // C++ language source code
		file_extension{".cmake"}, // CMake module or script
		file_extension{".cs"}, // Microsoft Visual Studio Visual C#.NET source code
		file_extension{".conf"}, // Configuration information
		file_extension{".cpp"}, // C++ source code file format
		file_extension{".css"}, // Cascading Style Sheets
		file_extension{".csv"}, // Comma Separated Value file
		file_extension{".cxx"}, // C++ source code file format
		file_extension{".d"}, // D Programming Language source code
		file_extension{".f"}, file_extension{".fpp"}, // Fortran source code
		file_extension{".fs"}, // Microsoft Visual F# source code
		file_extension{".go"}, // Google Go programming language source code
		file_extension{".h"}, // C header data
		file_extension{".hpp"}, // C++ header data
		file_extension{".htm"}, file_extension{".html"}, // HyperText Markup Language web page
		file_extension{".hxx"}, // C++ header data
		file_extension{".java"}, // Java language source code
		file_extension{".js"}, // JavaScript source code script
		file_extension{".json"}, // JavaScript object notation data interchange format
		file_extension{".jsp"}, // JAVA Server page file
		file_extension{".log"}, // Log
		file_extension{".lua"}, // Lua script
		file_extension{".md"}, // Markdown markup language source code
		file_extension{".pas"}, // Delphi unit source code
		file_extension{".php"}, // PHP script or page
		file_extension{".pl"}, file_extension{".perl"}, // Perl script language source code
		file_extension{".py"}, // Python script language source code
		file_extension{".r"}, // R script
		file_extension{".rss"}, // Really Simple Syndication - RSS file format
		file_extension{".sh"}, // Unix Bourne Shell (Bash) script
		file_extension{".tcl"}, // TCL script source code
		file_extension{".txt"}, file_extension{".text"}, // Simple text
		file_extension{".vb"}, file_extension{".vbs"}, // Visual Basic script
		file_extension{".xsd"}, // XML schema description
		file_extension{".xsl"}, // XML eXtensible stylesheet
		file_extension{".yml"}, file_extension{".yaml"}, // YAML document
		file_extension{".ws"} // Microsoft Windows script
	};
}

} // namespace docwire
