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

#include "rtf_parser.h"

#include "data_stream.h"
#include "error_tags.h"
#include "log.h"
#include <map>
#include "misc.h"
#include <mutex>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "throw_if.h"
#include <wv2/textconverter.h>
#include <wv2/ustring.h>
#include <boost/signals2.hpp>

namespace docwire
{

using namespace wvWare;

RTFParser::RTFParser()
{
}

#define RTFNAMEMAXLEN 32
#define RTFARGSMAXLEN 64

static long parseNumber(DataStream& data_stream)
{
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

int parseCharCode(DataStream& data_stream)
{
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

enum RTFCommand
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

struct RTFStringCommand
{
	const char* name;
	RTFCommand cmd;
};

RTFStringCommand rtf_commands[] =
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
	{ "emspace", RTF_EMDASH},
	{ "enspace", RTF_ENDASH},
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

struct RTFGroup
{
	int uc;
	//int codepage;
	destination_type destination;
};

struct RTFParserState
{
	std::stack<RTFGroup> groups;
	long int last_font_ref_num;
	std::map<long int, std::string> font_table;
	std::string author_of_next_annotation;
	tm annotation_time;
	UString annotation_text;
	UString fldinst_text;
	UString fldrslt_text;
};

static RTFCommand commandNameToEnum(char* name)
{
	for (int i = 0; i < sizeof(rtf_commands) / sizeof(RTFStringCommand); i++)
	{
		if (strcmp(name, rtf_commands[i].name) == 0)
			return rtf_commands[i].cmd;
	}
	return RTF_UNKNOWN;
}

static bool parseCommand(DataStream& data_stream, RTFCommand& cmd, long int& arg)
{
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
		cmd = commandNameToEnum(name);
		data_stream.unGetc(ch);
		if (isdigit(ch) || ch == '-' )
			arg = parseNumber(data_stream);
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
			arg = parseCharCode(data_stream);
			if(data_stream.eof())
				return false;
		}
		else
		{
			cmd = RTF_SPEC_CHAR;
			arg = ch;
		}
	}
	docwire_log(debug) << "[cmd: " << name << " (" << arg << ")]";
	return true;
}

static std::string codepage_to_encoding(int codepage)
{
	char encoding[7];
	snprintf(encoding, 7, "CP%i", codepage);
	return encoding;
}

static std::string win_charset_to_encoding(long int win_charset)
{
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
	return codepage_to_encoding(codepage);
}

static void parse_dttm_time(int dttm, tm& tm)
{
	tm.tm_sec = 0;
	tm.tm_min = dttm & 0x0000003F;
	dttm >>= 6;
	tm.tm_hour = dttm & 0x0000001F;
	dttm >>= 5;
	tm.tm_mday = dttm & 0x0000001F;
	dttm >>= 5;
	tm.tm_mon = dttm & 0x0000000F - 1;
	dttm >>= 4;
	tm.tm_year = dttm & 0x000001FF;
}

static void execCommand(DataStream& data_stream, UString& text, int& skip, RTFParserState& state, RTFCommand cmd, long int arg,
	TextConverter*& converter, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
	switch (cmd)
	{
		case RTF_SPEC_CHAR:
			if (arg == '*' && skip == 0)
				skip = state.groups.size() - 1;
			else if (arg == '\r' || arg == '\n') // the same as \para command
				text += UString("\n");
			else if (arg == '~')
				text += UString((UChar)0xA0); /* No-break space */
			else if (arg == '-')
				text += UString((UChar)0xAD); /* Optional hyphen */
			break;
		case RTF_EMDASH:
			text += UString((UChar)0x2014);
			break;
		case RTF_ENDASH: 
			text += UString((UChar)0x2013);
			break;
		case RTF_BULLET: 
			text += UString((UChar)0x2022);
			break;
		case RTF_LQUOTE:
			text += UString((UChar)0x2018);
			break;
		case RTF_RQUOTE:
			text += UString((UChar)0x2019);
			break;
		case RTF_LDBLQUOTE:
			text += UString((UChar)0x201C);
			break;
		case RTF_RDBLQUOTE:
			text += UString((UChar)0x201D);
			break;
		case RTF_ZWNONJOINER:
			text += UString((UChar)0xfeff);
			break;
		case RTF_EMSPACE:
		case RTF_ENSPACE:
			text += UString(' ');
			break;
		case RTF_CHAR:
			if (skip == 0)
			{
				if (converter != NULL)
					text += converter->convert((const char*)&arg, sizeof(arg));
				else
					text += UString((UChar)arg);
			}
			break;
		case RTF_UC:
			state.groups.top().uc = arg;
			break;
		case RTF_TAB:
			text += UString((UChar)0x0009);
			break;
		case RTF_UNICODE_CHAR:
			if (arg < 0)
				break;
			if (skip == 0)
				text += UString((UChar)arg);
			for (int i = 0; i < state.groups.top().uc; i++)
			{
				char ch = data_stream.getc();
				if (ch == '\\')
				{
					RTFCommand tmp_cmd;
					long int tmp_arg;
					parseCommand(data_stream, tmp_cmd, tmp_arg);
				}
			}
			break;
		case RTF_PARA:
			text += UString("\n");
			break;
		case RTF_CELL:
			text += UString("\n");
			break;
		case RTF_FLDINST:
			state.groups.top().destination = destination_type::fldinst;
			state.fldinst_text = "";
			skip = 0;
			break;
		case RTF_FLDRSLT:
			state.groups.top().destination = destination_type::fldrslt;
			state.fldrslt_text = "";
			skip = 0;
			break;
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
			if (skip == 0)
				skip = state.groups.size() - 1;
			break;
		case RTF_LANG:
			break;
		case RTF_LINE:
			text += UString("\n");
			break;
		case RTF_CODEPAGE:
			docwire_log(debug) << "Initializing converter for codepage " << arg;
			converter = new TextConverter(codepage_to_encoding(arg));
			if (converter->isOk())
			{
				docwire_log(debug) << "Converter initialized.";
			}
			else
			{
				non_fatal_error_handler(make_error_ptr("Converter initialization error"));
				delete converter;
				converter = NULL;
			}
			break;
		case RTF_FONT_CHARSET:
			docwire_log(debug) << "Setting win charset " << arg << " for font number " << state.last_font_ref_num;
			state.font_table[state.last_font_ref_num] = win_charset_to_encoding(arg);
			break;
		case RTF_F:
			if (state.font_table.find(arg) != state.font_table.end())
			{
				docwire_log(debug) << "Font number " << arg << " referenced. Setting converter for encoding " << state.font_table[arg];
				if (converter != NULL)
					converter->setFromCode(state.font_table[arg]);
			}
			else
				state.last_font_ref_num = arg;
			break;

		case RTF_ANNOTATION:
			state.groups.top().destination = destination_type::annotation;
			state.annotation_text = "";
			skip = 0;
			break;

		case RTF_ATNDATE:
		{
			char ch;
			std::string s;
			while (isdigit(ch = data_stream.getc()) && !data_stream.eof())
				s += ch;
			if (!data_stream.eof())
				data_stream.unGetc(ch);
			parse_dttm_time(str_to_int(s), state.annotation_time);
			break;
		}

		case RTF_ATNAUTHOR:
		{
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

namespace
{
	std::mutex converter_mutex;
} // anonymous namespace

void RTFParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using RTF parser.";
	UString text;
	std::span<const std::byte> span = data.span();
	auto stream = std::make_unique<BufferStream>(reinterpret_cast<const char*>(span.data()), span.size());
	TextConverter* converter = NULL;
	try
	{
		sendTag(tag::Document
			{
				.metadata = [this, &data]() { return metaData(data); }
			});
		char ch;
		RTFParserState state;
		state.groups.push(RTFGroup());
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
					RTFCommand cmd;
					long int arg;
					if (!parseCommand(*stream, cmd, arg))
						break;
					UString fragment_text;
					{
						std::lock_guard<std::mutex> converter_mutex_lock(converter_mutex);
						execCommand(*stream, fragment_text, skip, state, cmd, arg, converter, [this](std::exception_ptr e) { sendTag(e); });
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
						sendTag(tag::Comment{.author = state.author_of_next_annotation, .time = date_to_string(state.annotation_time), .comment = ustring_to_string(state.annotation_text)});
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
							if (url.front() == '"')
								url = url.substr(1);
							if (url.back() == ' ')
								url = url.substr(0, url.size() - 1);
							if (url.back() == '"')
								url = url.substr(0, url.size() - 1);
							sendTag(tag::Link{.url = url});
							sendTag(tag::Text{.text = ustring_to_string(state.fldrslt_text)});
							sendTag(tag::CloseLink{});
						}
					}
					if (skip > state.groups.size() - 1)
						skip = 0;
					if (!text.isEmpty())
					{
						sendTag(tag::Text({.text = ustring_to_string(text)}));
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
		sendTag(tag::CloseDocument{});
	}
	catch (std::bad_alloc& ba)
	{
		if (converter)
			delete converter;
		converter = NULL;
		throw;
	}
}

static bool parse_rtf_time(const std::string& s, tm& time)
{
	time = tm();
	size_t p2 = s.find("\\yr");
	if (p2 != std::string::npos)
	{
		std::istringstream(s.substr(p2 + 3)) >> time.tm_year;
		if (time.tm_year == 0)
		{
			// Sometimes field exists but date is zero.
			// Last modification time saved by LibreOffice 3.5 when document is created is an example.
			return false;
		}
		time.tm_year -= 1900;
	}
	p2 = s.find("\\mo");
	if (p2 != std::string::npos)
	{
		std::istringstream(s.substr(p2 + 3)) >> time.tm_mon;
		time.tm_mon--;
	}
	p2 = s.find("\\dy");
	if (p2 != std::string::npos)
		std::istringstream(s.substr(p2 + 3)) >> time.tm_mday;
	p2 = s.find("\\hr");
	if (p2 != std::string::npos)
		std::istringstream(s.substr(p2 + 3)) >> time.tm_hour;
	p2 = s.find("\\min");
	if (p2 != std::string::npos)
		std::istringstream(s.substr(p2 + 4)) >> time.tm_min;
	p2 = s.find("\\sec");
	if (p2 != std::string::npos)
		std::istringstream(s.substr(p2 + 4)) >> time.tm_sec;
	return true;
}

attributes::Metadata RTFParser::metaData(const data_source& data) const
{	
	attributes::Metadata meta;
	docwire_log(debug) << "Extracting metadata.";
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
		tm creation_date;
		if (parse_rtf_time(s, creation_date))
			meta.creation_date = creation_date;
	}
	p = content.find("\\revtim");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 7; content[i] != '}'; i++)
			s += content[i];
		tm last_modification_date;
		if (parse_rtf_time(s, last_modification_date))
			meta.last_modification_date = last_modification_date;
	}
	p = content.find("\\nofpages");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 9; content[i] != '}'; i++)
			s += content[i];
		int page_count;
		std::istringstream(s) >> page_count;
		meta.page_count = page_count;
	}
	p = content.find("\\nofwords");
	if (p != std::string::npos)
	{
		std::string s;
		for (int i = p + 9; content[i] != '}'; i++)
			s += content[i];
		int word_count;
		std::istringstream(s) >> word_count;
		meta.word_count = word_count;
	}
	return meta;
}

} // namespace docwire
