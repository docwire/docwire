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
    void parse(const data_source& data, const emission_callbacks& emit_tag);
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

const std::vector<mime_type> supported_mime_types =
{
    mime_type{"text/x-asm"},
    mime_type{"text/asp"},
    mime_type{"text/aspdotnet"},
    mime_type{"text/x-basic"},
    mime_type{"text/x-bat"},
    mime_type{"text/x-c"},
    mime_type{"text/x-cmake"},
    mime_type{"text/x-csharp"},
    mime_type{"text/css"},
    mime_type{"text/csv"},
    mime_type{"text/x-d"},
    mime_type{"text/x-fortran"},
    mime_type{"text/x-fsharp"},
    mime_type{"text/x-go"},
    mime_type{"text/x-c++hdr"},
    mime_type{"text/html"},
    mime_type{"text/x-java-source"},
    mime_type{"application/javascript"},
    mime_type{"text/javascript"},
    mime_type{"application/json"},
    mime_type{"text/x-jsp"},
    mime_type{"text/x-lua"},
    mime_type{"text/markdown"},
    mime_type{"text/x-pascal"},
    mime_type{"application/x-httpd-php"},
    mime_type{"text/x-perl"},
    mime_type{"text/x-python"},
    mime_type{"text/x-rsrc"},
    mime_type{"application/rss+xml"},
    mime_type{"application/x-sh"},
    mime_type{"application/x-tcl"},
    mime_type{"text/plain"},
    mime_type{"text/x-vbdotnet"},
    mime_type{"text/x-vbscript"},
    mime_type{"application/xml"},
    mime_type{"text/yaml"}
};

} // anonymous namespace

void pimpl_impl<TXTParser>::parse(const data_source& data, const emission_callbacks& emit_tag)
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
			emit_tag(make_error_ptr("Could not create charset detector"));
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
				emit_tag(make_nested_ptr(std::current_exception(), make_error("Cannot convert text to UTF-8", encoding)));
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
	bool parse_paragraphs = m_parse_paragraphs.v;
	bool parse_lines = m_parse_lines.v;
	emit_tag(tag::Document{});
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
					emit_tag(tag::Paragraph{});
					paragraph_state = empty_paragraph;
				}
				if (line.empty())
				{
					emit_tag(tag::CloseParagraph{});
					paragraph_state = outside_paragraph;
				}
				else
				{
					if (paragraph_state == filled_paragraph)
					{
						if (parse_lines)
							emit_tag(tag::BreakLine{});
						else
							emit_tag(tag::Text{.text = last_eol});
					}
					emit_tag(tag::Text{.text = line});
					paragraph_state = filled_paragraph;
				}
			}
			else
			{
				if (!line.empty())
					emit_tag(tag::Text{.text = line});
				if (!eol.empty())
				{
					if (parse_lines)
						emit_tag(tag::BreakLine{});
					else
						emit_tag(tag::Text{.text = eol});
				}
			}
			if (eol.empty())
				break;
			curr_pos = eol_pos + eol.size();
			last_eol = eol;
		}
		if (parse_paragraphs && paragraph_state != outside_paragraph)
			emit_tag(tag::CloseParagraph{});
	}
	else
		emit_tag(tag::Text{.text = text});
	emit_tag(tag::CloseDocument{});
}

continuation TXTParser::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
  if (!std::holds_alternative<data_source>(tag))
    return emit_tag(std::move(tag));

  auto& data = std::get<data_source>(tag);
  data.assert_not_encrypted();

  if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
    return emit_tag(std::move(tag));

  try
  {
      impl().parse(data, emit_tag);
  }
  catch (const std::exception& e)
  {
    std::throw_with_nested(make_error("Parsing failed"));
  }
  return continuation::proceed;
}

} // namespace docwire
