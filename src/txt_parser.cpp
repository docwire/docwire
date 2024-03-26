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
#include "data_stream.h"
#include "exception.h"
#include "htmlcxx/html/CharsetConverter.h"
#include <boost/signals2.hpp>
#include <iostream>
#include "log.h"
#include <string.h>

namespace docwire
{

struct TXTParser::Implementation
{
	std::string m_file_name;
	DataStream* m_data_stream;
	boost::signals2::signal<void(Info &info)> m_on_new_node_signal;
};

TXTParser::TXTParser(const std::string& file_name, const Importer* inImporter)
: Parser(inImporter)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_data_stream = NULL;
		impl->m_data_stream = new FileStream(file_name);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

TXTParser::TXTParser(const char* buffer, size_t size, const Importer* inImporter)
: Parser(inImporter)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_data_stream = NULL;
		impl->m_data_stream = new BufferStream(buffer, size);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

TXTParser::~TXTParser()
{
	if (impl)
	{
		if (impl->m_data_stream)
			delete impl->m_data_stream;
		delete impl;
	}
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

std::string TXTParser::plainText() const
{
	std::string text;
	csd_t charset_detector = NULL;
	htmlcxx::CharsetConverter* converter = NULL;
	try
	{
		std::string encoding;
		if (!impl->m_data_stream->open())
			throw RuntimeError("Could not open file: " + impl->m_file_name);
		std::string content;
		size_t file_size = impl->m_data_stream->size();
		content.resize(file_size);
		if (!impl->m_data_stream->read(&content[0], 1, file_size))
			throw RuntimeError("Could not read from file: " + impl->m_file_name);
		impl->m_data_stream->close();
		charset_detector = csd_open();
		if (charset_detector == (csd_t)-1)
		{
			charset_detector = NULL;
			docwire_log(warning) << "Warning: Could not create charset detector";
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
				docwire_log(warning) << "Warning: Cant convert text to UTF-8 from " + encoding;
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
		impl->m_data_stream->close();
		if (converter)
			delete converter;
		converter = NULL;
		if (charset_detector)
			csd_close(charset_detector);
		charset_detector = NULL;
		throw RuntimeError("Could not parse text: " + std::string(e.what()));
	}
	text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
	return text;
}

Parser&
TXTParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
	return *this;
}

void
TXTParser::parse() const
{
	docwire_log(debug) << "Using TXT parser.";

  Info info(tag::Text{.text = plainText()});
  impl->m_on_new_node_signal(info);
}

TXTParser::Parser&
TXTParser::addOnNewNodeCallback(NewNodeCallback callback)
{
  impl->m_on_new_node_signal.connect(callback);
  return *this;
}

std::vector <std::string> TXTParser::getExtensions()
{
	return
	{
		"asm", // Assembler source code
		"asp", // Active Server Page script page
		"aspx", // Active Server Page Extended ASP.NET script
		"bas", // Basic source code
		"bat", // Batch file (script)
		"c", // C source code
		"cc", // C++ language source code
		"cmake", // CMake module or script
		"cs", // Microsoft Visual Studio Visual C#.NET source code
		"conf", // Configuration information
		"cpp", // C++ source code file format
		"css", // Cascading Style Sheets
		"csv", // Comma Separated Value file
		"cxx", // C++ source code file format
		"d", // D Programming Language source code
		"f", "fpp", // Fortran source code
		"fs", // Microsoft Visual F# source code
		"go", // Google Go programming language source code
		"h", // C header data
		"hpp", // C++ header data
		"htm", "html", // HyperText Markup Language web page
		"hxx", // C++ header data
		"java", // Java language source code
		"js", // JavaScript source code script
		"json", // JavaScript object notation data interchange format
		"jsp", // JAVA Server page file
		"log", // Log
		"lua", // Lua script
		"md", // Markdown markup language source code
		"pas", // Delphi unit source code
		"php", // PHP script or page
		"pl", "perl", // Perl script language source code
		"py", // Python script language source code
		"r", // R script
		"rss", // Really Simple Syndication - RSS file format
		"sh", // Unix Bourne Shell (Bash) script
		"tcl", // TCL script source code
		"txt", "text", // Simple text
		"vb", "vbs", // Visual Basic script
		"xml", // XML document
		"xsd", // XML schema description
		"xsl", // XML eXtensible stylesheet
		"yml", "yaml", // YAML document
		"ws" // Microsoft Windows script
	};
}

} // namespace docwire
