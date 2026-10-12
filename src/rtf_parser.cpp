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

#include "rtf_parser.h"

#include "convert_chrono.h" // IWYU pragma: keep
#include "convert_numeric.h" // IWYU pragma: keep
#include "document_elements.h"
#include "data_stream.h"
#include "data_source.h"
#include "error_tags.h"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "serialization_message.h" // IWYU pragma: keep
#include <map>
#include "detail/ustring_conversions.h"
#include <mutex>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serialization_data_source.h" // IWYU pragma: keep
#include "serialization_enum.h" // IWYU pragma: keep
#include <time.h>
#include "throw_if.h"
#include "wv2/src/textconverter.h"
#include "wv2/src/ustring.h"

namespace docwire
{

using namespace wvWare;

#define RTFNAMEMAXLEN 32
#define RTFARGSMAXLEN 64

namespace
{

template<typename auditor_type>
long parseNumber(data_stream& data_stream, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, audit::no_context};
	int ch;
	int count = 0;
	char buf[RTFARGSMAXLEN + 1];
	
	while (isdigit(ch = data_stream.getc()) || ch == '-')
	{
		if(data_stream.eof())
			return -1;
		if (count < RTFARGSMAXLEN)
			buf[count++] = (char)ch;
	}
	data_stream.unGetc(ch);
	buf[count] = '\0';
	return strtol(buf, (char **)NULL, 10);
}

template<typename auditor_type>
int parseCharCode(data_stream& data_stream, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, audit::no_context};
	int ch;
	int count = 0;
	char buf[RTFARGSMAXLEN + 1];

	for (int i = 0; i < 2; i++)
	{
		if (isdigit(ch = data_stream.getc()) || (ch >= 'a' && ch <= 'f'))
		{
			if(data_stream.eof())
				return -1;
			if (count < RTFARGSMAXLEN)
				buf[count++] = (char)ch;
		}
		else
			data_stream.unGetc(ch);
	}

	buf[count]='\0';
	return strtol(buf, (char **)NULL, 16);
}

enum rtf_command
{
	RTF_CODEPAGE,
	RTF_FONT_CHARSET,
	RTF_UC,
	RTF_UNICODE_CHAR,
	RTF_CHAR,
	RTF_PARA,
	RTF_TABLE_START,
	RTF_TABLE_END,
	RTF_ROW,
	RTF_CELL,
	RTF_UNKNOWN,
	RTF_OVERLAY,
	RTF_PICT,
	RTF_F,
	RTF_AUTHOR,
	RTF_FONTTBL,
	RTF_INFO,
	RTF_STYLESHEET,
	RTF_COLORTBL,
	RTF_LISTOVERRIDETABLE,
	RTF_LISTTABLE,
	RTF_RSIDTBL,
	RTF_GENERATOR,
	RTF_DATAFIELD,
	RTF_LANG,
	RTF_LINE,
	RTF_PARD,
	RTF_TAB,
	RTF_SPEC_CHAR,
	RTF_EMDASH,
	RTF_ENDASH,
	RTF_EMSPACE,
	RTF_ENSPACE,
 	RTF_BULLET, 
 	RTF_LQUOTE,
	RTF_RQUOTE,
	RTF_LDBLQUOTE,
	RTF_RDBLQUOTE,
	RTF_ZWNONJOINER,
	RTF_FLDINST,
	RTF_FLDRSLT,
	RTF_ANNOTATION,
	RTF_ATNDATE,
	RTF_ATNAUTHOR
};

struct rtf_string_command
{
	const char* name;
	rtf_command cmd;
};

rtf_string_command rtf_commands[] =
{
	{ "uc", RTF_UC},
	{ "ansicpg", RTF_CODEPAGE},
	{ "pard", RTF_PARD},
	{ "par", RTF_PARA},
	{ "cell", RTF_CELL},
	{ "row", RTF_ROW},
	{ "overlay", RTF_OVERLAY}, 
	{ "pict" ,RTF_PICT},
	{ "author", RTF_AUTHOR},
	{ "f", RTF_F}, 
	{ "fonttbl", RTF_FONTTBL}, 
	{ "info", RTF_INFO}, 
	{ "stylesheet", RTF_STYLESHEET},
	{ "colortbl", RTF_COLORTBL},
	{ "line", RTF_LINE},
	{ "listtable", RTF_LISTTABLE},
	{ "listoverridetable", RTF_LISTOVERRIDETABLE},
	{ "rsidtbl", RTF_RSIDTBL}, 
	{ "generator", RTF_GENERATOR}, 
	{ "datafield", RTF_DATAFIELD}, 
	{ "lang", RTF_LANG}, 
	{ "tab", RTF_TAB}, 
	{ "emdash", RTF_EMDASH},
	{ "endash", RTF_ENDASH},
	{ "emspace", RTF_EMSPACE},
	{ "enspace", RTF_ENSPACE},
	{ "bullet", RTF_BULLET}, 
	{ "lquote", RTF_LQUOTE},
	{ "rquote", RTF_RQUOTE},
	{ "ldblquote", RTF_LDBLQUOTE},
	{ "rdblquote", RTF_RDBLQUOTE},
	{ "zwnj", RTF_ZWNONJOINER},
	{ "u", RTF_UNICODE_CHAR},
	{ "fldinst", RTF_FLDINST},
	{ "fldrslt", RTF_FLDRSLT},
	{ "fcharset", RTF_FONT_CHARSET },
	{ "annotation", RTF_ANNOTATION },
	{ "atndate", RTF_ATNDATE },
	{ "atnauthor", RTF_ATNAUTHOR }
};

enum class destination_type { text, annotation, fldinst, fldrslt };

struct rtf_group
{
	int uc;
	//int codepage;
	destination_type destination;
};

struct rtf_parser_state
{
	std::stack<rtf_group> groups;
	long int last_font_ref_num;
	std::map<long int, std::string> font_table;
	std::string author_of_next_annotation;
	std::chrono::sys_seconds annotation_time;
	UString annotation_text;
	UString fldinst_text;
	UString fldrslt_text;
};

template<typename auditor_type>
rtf_command commandNameToEnum(char* name, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "name"_v = name; }};
	for (int i = 0; i < sizeof(rtf_commands) / sizeof(rtf_string_command); i++)
	{
		if (strcmp(name, rtf_commands[i].name) == 0)
			return rtf_commands[i].cmd;
	}
	return RTF_UNKNOWN;
}

template<typename auditor_type>
bool parseCommand(data_stream& data_stream, rtf_command& cmd, long int& arg,
                  audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, audit::no_context};
	char name[RTFNAMEMAXLEN + 1];

	int ch = data_stream.getc();
	if (isalpha(ch))
	{
		int name_count = 1;
		name[0] = (char)ch;
		while (isalpha(ch = data_stream.getc()) && name_count < RTFNAMEMAXLEN)
		{
			if(data_stream.eof())
				return false;
			name[name_count++] = (char)ch;
		}
		name[name_count] = '\0';
		cmd = commandNameToEnum(name, auditor);
		data_stream.unGetc(ch);
		if (isdigit(ch) || ch == '-' )
			arg = parseNumber(data_stream, auditor);
		else
			arg = 0;
		ch = data_stream.getc();
		if(!(ch == ' ' || ch == '\t'))
			data_stream.unGetc(ch);
	}
	else
	{
		name[0] = (char)ch;
		name[1] = '\0';
		if (ch == '\'')
		{
			cmd = RTF_CHAR;
			arg = parseCharCode(data_stream, auditor);
			if(data_stream.eof())
				return false;
		}
		else
		{
			cmd = RTF_SPEC_CHAR;
			arg = ch;
		}
	}
	audit::entry(auditor, [&]{
		return "name"_v = std::string{name}, "arg"_v = arg;
	});
	return true;
}

template<typename auditor_type>
std::string codepage_to_encoding(int codepage, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "codepage"_v = codepage; }};
	char encoding[7];
	snprintf(encoding, 7, "CP%i", codepage);
	return encoding;
}

template<typename auditor_type>
std::string win_charset_to_encoding(long int win_charset, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "win_charset"_v = win_charset; }};
	long int codepage = 1250;
	switch (win_charset)
	{
		case 0: codepage = 1250; break; // ANSI_CHARSET
		case 1: codepage = 1250; break; // DEFAULT_CHARSET
		//case 2: codepage = ; break; // SYMBOL_CHARSET not supported yet.
		case 77: codepage = 10000; break; // MAC_CHARSET
		case 128: codepage = 932; break; // SHIFTJIS_CHARSET
		case 129: codepage = 949; break; // HANGEUL_CHARSET
		case 130: codepage = 1361; break; // JOHAB_CHARSET
		case 134: codepage = 936; break; // GB2312_CHARSET
		case 136: codepage = 950; break; // CHINESEBIG5_CHARSET
		case 161: codepage = 1253; break; // GREEK_CHARSET
		case 162: codepage = 1254; break; // TURKISH_CHARSET
		case 163: codepage = 1258; break; // VIETNAMESE_CHARSET
		case 177: codepage = 1255; break; // HEBREW_CHARSET
		case 178: codepage = 1256; break; // ARABIC_CHARSET / ARABICSIMPLIFIED_CHARSET
		case 186: codepage = 1257; break; // BALTIC_CHARSET
		case 204: codepage = 1251; break; // RUSSIAN_CHARSET / CYRILLIC_CHARSET
		case 222: codepage = 874; break; // THAI_CHARSET
		case 238: codepage = 1250; break; // EASTEUROPE_CHARSET / 
		case 255: codepage = 850; break; // OEM_CHARSET
	}
	return codepage_to_encoding(codepage, auditor);
}

template<typename auditor_type>
std::chrono::sys_seconds parse_dttm_time(int dttm, audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "dttm"_v = dttm; }};
	int min = dttm & 0x0000003F;
	dttm >>= 6;
	int hour = dttm & 0x0000001F;
	dttm >>= 5;
	int day = dttm & 0x0000001F;
	dttm >>= 5;
	int mon = dttm & 0x0000000F;
	dttm >>= 4;
	int year = 1900 + (dttm & 0x000001FF);

	using namespace std::chrono;
	year_month_day ymd{std::chrono::year{year}, std::chrono::month{static_cast<unsigned>(mon)}, std::chrono::day{static_cast<unsigned>(day)}};
	if (!ymd.ok()) return {};
	return sys_days{ymd} + hours{hour} + minutes{min};
}

template<typename auditor_type>
void execCommand(
	data_stream& data_stream,
	UString& text,
	int& skip,
	rtf_parser_state& state,
	rtf_command cmd,
	long int arg,
	TextConverter*& converter,
	const std::function<void(std::exception_ptr)>& non_fatal_error_handler,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{
		return "cmd"_v = cmd, "arg"_v = arg;
	}};
	switch (cmd)
	{
		case RTF_SPEC_CHAR:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			if (arg == '*' && skip == 0)
				skip = state.groups.size() - 1;
			else if (arg == '\r' || arg == '\n') // the same as \para command
				text += UString("\n");
			else if (arg == '~')
				text += UString((UChar)0xA0); /* No-break space */
			else if (arg == '-')
				text += UString((UChar)0xAD); /* Optional hyphen */
			break;
		}
		case RTF_EMDASH:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x2014);
			break;
		}
		case RTF_ENDASH:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x2013);
			break;
		}
		case RTF_BULLET:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x2022);
			break;
		}
		case RTF_LQUOTE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x2018);
			break;
		}
		case RTF_RQUOTE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x2019);
			break;
		}
		case RTF_LDBLQUOTE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x201C);
			break;
		}
		case RTF_RDBLQUOTE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x201D);
			break;
		}
		case RTF_ZWNONJOINER:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0xfeff);
			break;
		}
		case RTF_EMSPACE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString(' ');
			break;
		}
		case RTF_ENSPACE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString(' ');
			break;
		}
		case RTF_CHAR:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			if (skip == 0)
			{
				if (converter != NULL)
					text += converter->convert((const char*)&arg, sizeof(arg));
				else
					text += UString((UChar)arg);
			}
			break;
		}
		case RTF_UC:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			state.groups.top().uc = arg;
			break;
		}
		case RTF_TAB:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString((UChar)0x0009);
			break;
		}
		case RTF_UNICODE_CHAR:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			if (arg < 0)
				break;
			if (skip == 0)
				text += UString((UChar)arg);
			for (int i = 0; i < state.groups.top().uc; i++)
			{
				char ch = data_stream.getc();
				if (ch == '\\')
				{
					rtf_command tmp_cmd;
					long int tmp_arg;
					parseCommand(data_stream, tmp_cmd, tmp_arg, auditor);
				}
			}
			break;
		}
		case RTF_PARA:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString("\n");
			break;
		}
		case RTF_CELL:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString("\n");
			break;
		}
		case RTF_FLDINST:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			state.groups.top().destination = destination_type::fldinst;
			state.fldinst_text = "";
			skip = 0;
			break;
		}
		case RTF_FLDRSLT:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			state.groups.top().destination = destination_type::fldrslt;
			state.fldrslt_text = "";
			skip = 0;
			break;
		}
		case RTF_PICT:
		case RTF_FONTTBL:
		case RTF_INFO:
		case RTF_COLORTBL:
		case RTF_STYLESHEET:
		case RTF_LISTTABLE:
		case RTF_LISTOVERRIDETABLE:
		case RTF_RSIDTBL:
		case RTF_GENERATOR:
		case RTF_DATAFIELD:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			if (skip == 0)
				skip = state.groups.size() - 1;
			break;
		}
		case RTF_LANG:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			break;
		}
		case RTF_LINE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			text += UString("\n");
			break;
		}
		case RTF_CODEPAGE:
		{
			audit::scope audit_scope{auditor, [&]{ return "arg"_v = arg; }};
			converter = new TextConverter(codepage_to_encoding(arg, auditor));
			if (converter->isOk())
			{
				audit::entry(auditor, audit::no_context);
			}
			else
			{
				non_fatal_error_handler(make_error_ptr("Converter initialization error"));
				delete converter;
				converter = NULL;
			}
			break;
		}
		case RTF_FONT_CHARSET:
		{
			audit::scope audit_scope{auditor, [&]{
				return "arg"_v = arg, "last_font_ref_num"_v = state.last_font_ref_num;
			}};
			state.font_table[state.last_font_ref_num] = win_charset_to_encoding(arg, auditor);
			break;
		}
		case RTF_F:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			if (state.font_table.find(arg) != state.font_table.end())
			{
				audit::entry(auditor, [&]{
					return "arg"_v = arg, "font"_v = state.font_table[arg];
				});
				if (converter != NULL)
					converter->setFromCode(state.font_table[arg]);
			}
			else
				state.last_font_ref_num = arg;
			break;
		}
		case RTF_ANNOTATION:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			state.groups.top().destination = destination_type::annotation;
			state.annotation_text = "";
			skip = 0;
			break;
		}
		case RTF_ATNDATE:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			char ch;
			std::string s;
			while (isdigit(ch = data_stream.getc()) && !data_stream.eof())
				s += ch;
			if (!data_stream.eof())
				data_stream.unGetc(ch);
			state.annotation_time = parse_dttm_time(convert::to<int>(s), auditor);
			break;
		}
		case RTF_ATNAUTHOR:
		{
			audit::scope audit_scope{auditor, audit::no_context};
			state.author_of_next_annotation = "";
			char ch;
			while ((ch = data_stream.getc()) != '}' && !data_stream.eof())
				state.author_of_next_annotation += ch;
			if (!data_stream.eof())
				data_stream.unGetc(ch);
			break;
		}
	}
}

std::mutex converter_mutex;

template<typename auditor_type>
attributes::metadata extract_rtf_metadata(
	const data_source& data,
	audit::auditor_ref<auditor_type> auditor); // Forward declaration

template<typename auditor_type>
void parse_rtf_content(
	const data_source& data,
	const message_callbacks& emit_message,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "data"_v = data; }};
	UString text;
	std::span<const std::byte> span = data.span();
	auto stream = std::make_unique<buffer_stream>(reinterpret_cast<const char*>(span.data()), span.size());
	TextConverter* converter = NULL;
	try
	{
		emit_message(document::document
			{
				.metadata = [&data, auditor]() { return extract_rtf_metadata(data, auditor); }
			});
		char ch;
		rtf_parser_state state;
		state.groups.push(rtf_group());
		state.groups.top().uc = 1;
		state.groups.top().destination = destination_type::text;
		state.last_font_ref_num = 0;
		int skip = 0;
		while ((ch = stream->getc()) != EOF)
		{
			switch (ch)
			{
				case '\\':
				{
					rtf_command cmd;
					long int arg;
					if (!parseCommand(*stream, cmd, arg, auditor))
						break;
					UString fragment_text;
					{
						std::lock_guard<std::mutex> converter_mutex_lock(converter_mutex);
						execCommand(
							*stream, fragment_text, skip, state, cmd, arg, converter,
							[emit_message](std::exception_ptr e) { emit_message(std::move(e)); },
							auditor);
					}
					switch (state.groups.top().destination)
					{
						case destination_type::annotation:
							state.annotation_text += fragment_text;
							break;
						case destination_type::fldinst:
							state.fldinst_text += fragment_text;
							break;
						case destination_type::fldrslt:
							state.fldrslt_text += fragment_text;
							break;
						default:
							text += fragment_text;
					}
					break;
				}

				case '{':
					state.groups.push(state.groups.top());
					//state.groups.top().destination = destination_type::text;
					break;
				case '}':
				{
					destination_type destination = state.groups.top().destination;
					state.groups.pop();
					if (destination == destination_type::annotation && state.groups.top().destination != destination_type::annotation)
						emit_message(document::comment{
							.author = state.author_of_next_annotation,
							.time = convert::to<std::string>(state.annotation_time),
							.comment = ustring_to_string(state.annotation_text)
						});
					else if (destination == destination_type::fldinst)
					{
					}
					else if (destination == destination_type::fldrslt && state.groups.top().destination != destination_type::fldrslt)
					{
						std::string fldinst_text = ustring_to_string(state.fldinst_text);
						if (fldinst_text.starts_with("HYPERLINK "))
						{
							std::string::size_type space_pos = fldinst_text.find(' ', 10);
							if (space_pos == std::string::npos)
								space_pos = fldinst_text.size();
							std::string url = fldinst_text.substr(10, space_pos - 10);

							// Trim leading whitespace.
							url.erase(0, url.find_first_not_of(" \t\n\r\f\v"));

							// Trim one leading quote.
							if (url.starts_with('"')) {
								url.erase(0, 1);
							}

							// Trim trailing whitespace and quotes.
							if (auto pos = url.find_last_not_of(" \t\n\r\f\v\""); pos != std::string::npos) {
								url.erase(pos + 1);
							} else {
								url.clear();
							}

							emit_message(document::link{.url = url});
							emit_message(document::text{.text = ustring_to_string(state.fldrslt_text)});
							emit_message(document::close_link{});
						}
					}
					if (skip > state.groups.size() - 1)
						skip = 0;
					if (!text.isEmpty())
					{
						emit_message(document::text({.text = ustring_to_string(text)}));
						text = "";
					}
					break;
				}

				default:
					if (skip == 0 && (ch != '\n' || state.groups.top().destination == destination_type::annotation) && ch != '\r')
					{
						UString fragment_text;
						if (converter != NULL)
							fragment_text = converter->convert((const char*)&ch, sizeof(ch));
						else
							fragment_text = UString((UChar)ch);
						switch (state.groups.top().destination)
						{
							case destination_type::annotation:
								state.annotation_text += fragment_text;
								break;
							case destination_type::fldinst:
								state.fldinst_text += fragment_text;
								break;
							case destination_type::fldrslt:
								state.fldrslt_text += fragment_text;
								break;
							default:
								text += fragment_text;
						}
					}
			}
			throw_if (state.groups.size() == 0, "File is corrupted", errors::uninterpretable_data{}); //it will crash soon if groups.size() returns zero... better to check
		}
		if (converter != NULL)
			delete converter;
		converter = NULL;
		emit_message(document::close_document{});
	}
	catch (std::bad_alloc& ba)
	{
		if (converter)
			delete converter;
		converter = NULL;
		throw;
	}
}

std::optional<int> get_rtf_date_component(std::string_view s, std::string_view tag)
{
    size_t pos = s.find(tag);
    if (pos == std::string_view::npos) return std::nullopt;

    std::string_view rest = s.substr(pos + tag.length());

    // Skip optional space which might delimit the control word
    if (!rest.empty() && rest.front() == ' ')
        rest.remove_prefix(1);

    return convert::try_to<int>(with::partial_match{rest});
}

template<typename auditor_type>
std::optional<std::chrono::sys_seconds> parse_rtf_time(
	std::string_view s,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "s"_v = s; }};
	int year = get_rtf_date_component(s, "\\yr").value_or(0);

	int month = get_rtf_date_component(s, "\\mo").value_or(1);
	int day = get_rtf_date_component(s, "\\dy").value_or(1);
	int hour = get_rtf_date_component(s, "\\hr").value_or(0);
	int minute = get_rtf_date_component(s, "\\min").value_or(0);
	int second = get_rtf_date_component(s, "\\sec").value_or(0);

	using namespace std::chrono;
	year_month_day ymd{std::chrono::year{year}, std::chrono::month{static_cast<unsigned>(month)}, std::chrono::day{static_cast<unsigned>(day)}};
	if (!ymd.ok()) return std::nullopt;
	return sys_days{ymd} + hours{hour} + minutes{minute} + seconds{second};
}

template<typename auditor_type>
attributes::metadata extract_rtf_metadata(
	const data_source& data,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "data"_v = data; }};
	attributes::metadata meta;
	std::string content = data.string();
	size_t p = content.find("\\author ");
	if (p != std::string::npos)
	{
		std::string author;
		for (int i = p + 8; content[i] != '{' && content[i] != '}'; i++)
			author += content[i];
		meta.author = author;
	}
	p = content.find("\\operator ");
	if (p != std::string::npos)
	{
		std::string last_modified_by;
		for (int i = p + 10; content[i] != '{' && content[i] != '}'; i++)
			last_modified_by += content[i];
		meta.last_modified_by = last_modified_by;
	}
	p = content.find("\\creatim");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 8; content[i] != '}'; i++)
			s += content[i];
		meta.creation_date = parse_rtf_time(s, auditor);
	}
	p = content.find("\\revtim");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 7; content[i] != '}'; i++)
			s += content[i];
		meta.last_modification_date = parse_rtf_time(s, auditor);
	}
	p = content.find("\\nofpages");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 9; content[i] != '}'; i++)
			s += content[i];
		meta.page_count = convert::try_to<int>(s);
	}
	p = content.find("\\nofwords");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 9; content[i] != '}'; i++)
			s += content[i];
		meta.word_count = convert::try_to<int>(s);
	}
	return meta;
}

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/rtf"},
	mime_type{"text/rtf"},
	mime_type{"text/richtext"}
};

} // anonymous namespace

template<typename auditor_type>
rtf_parser<auditor_type>::rtf_parser(audit::auditor_ref<auditor_type> auditor)
	: audit::with_auditor<auditor_type>(auditor)
{
}

template<typename auditor_type>
continuation rtf_parser<auditor_type>::operator()(
	message_ptr msg,
	const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};

	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	try
	{
		parse_rtf_content(data, emit_message, audit::auditor_ref<auditor_type>(auditor()));
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("RTF parsing failed"));
	}
	return continuation::proceed;
}

template class DOCWIRE_RTF_EXPORT rtf_parser<docwire::audit::null_auditor>;
template class DOCWIRE_RTF_EXPORT rtf_parser<docwire::audit::legacy_log_audit>;
template class DOCWIRE_RTF_EXPORT rtf_parser<docwire::audit::json_stream_auditor>;

} // namespace docwire
