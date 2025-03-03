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
#include "charset_converter.h"
#include "log.h"
#include "make_error.h"
#include "pimpl.h"
#include <string.h>

namespace docwire
{

template<>
struct pimpl_impl<TXTParser> : pimpl_impl_base
{
	pimpl_impl(parse_paragraphs parse_paragraphs_arg, parse_lines parse_lines_arg)
		: m_parse_paragraphs{parse_paragraphs_arg}, m_parse_lines{parse_lines_arg} {}
	parse_paragraphs m_parse_paragraphs;
	parse_lines m_parse_lines;
};

TXTParser::TXTParser(parse_paragraphs parse_paragraphs_arg, parse_lines parse_lines_arg)
	: with_pimpl<TXTParser>{parse_paragraphs_arg, parse_lines_arg} {}

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

void TXTParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using TXT parser.";
	std::string text;
	csd_t charset_detector = NULL;
	charset_converter* converter = NULL;
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
				converter = new charset_converter(encoding, "UTF-8");
			}
			catch (std::exception&)
			{
				sendTag(make_nested_ptr(std::current_exception(), make_error("Cannot convert text to UTF-8", encoding)));
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
	bool parse_paragraphs = impl().m_parse_paragraphs.v;
	bool parse_lines = impl().m_parse_lines.v;
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

} // namespace docwire
