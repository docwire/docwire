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

#include "xml_fixer.h"

#include <algorithm>
#include <libxml/parserInternals.h> // xmlParserMaxDepth
#include <list>
#include <sstream>
#include <stack>

namespace docwire
{

struct xml_name
{
	std::string nspace;
	std::string name;

	xml_name()
	{
	}

	xml_name(const std::string& _nspace, const std::string _name)
		: nspace(_nspace), name(_name)
	{
	}

	bool operator==(const xml_name& right) const
	{
		return nspace == right.nspace && name == right.name;
	}

	std::string fullName() const
	{
		std::string full_name;
		if (nspace != "")
			full_name += nspace + ":";
		full_name += name;
		return full_name;
	}
};

struct attr
{
	xml_name name;
	std::string value;

	attr()
	{
	}

	attr(const xml_name& _name, const std::string& _value)
		: name(_name), value(_value)
	{
	}

	std::string toText() const
	{
		return name.fullName() + "=\"" + value + "\"";
	}

	/**
		Required for std::find.
	**/
	bool operator==(const attr& rhs)
	{
		return name == rhs.name;
	}
};

struct xml_tag
{
	enum tag_type { HEADER, OPENING, CLOSING, OPENING_AND_CLOSING };
	tag_type type;
	xml_name name;
	std::list<attr> attrs;

	xml_tag()
	{
	}

	xml_tag(tag_type _type, const xml_name& _name)
		: type(_type), name(_name)
	{
	}

	std::string toText() const
	{
		std::string text = "<";
		if (type == CLOSING)
			text += '/';
		else if (type == HEADER)
			text += '?';
		text += name.fullName();
		for (std::list<attr>::const_iterator i = attrs.begin(); i != attrs.end(); i++)
			text += " " + (*i).toText();
		if (type == OPENING_AND_CLOSING)
			text += '/';
		else if (type == HEADER)
			text += '?';
		text += ">";
		return text;
	}
};

template<>
struct pimpl_impl<xml_fixer> : pimpl_impl_base
{
	std::istringstream xml;

	bool parseChar(char* ch)
	{
		int res = xml.get();
		if (res == EOF)
			return false;
		*ch = res;
		return true;
	}

	bool parseAssertChar(char assert_ch)
	{
		char ch;
		if (!parseChar(&ch))
			return false;
		if (ch == assert_ch)
			return true;
		xml.seekg(-1, std::ios_base::cur);
		return false;
	}

	bool parseAlpha(char* ch)
	{
		if (!parseChar(ch))
			return false;
		if (isalpha(*ch))
			return true;
		xml.seekg(-1, std::ios_base::cur);
		return false;
	}

	bool parseAlnumMinus(char* ch)
	{
		if (!parseChar(ch))
			return false;
		if (isalnum(*ch) || *ch == '-')
			return true;
		xml.seekg(-1, std::ios_base::cur);
		return false;
	}

	bool parseName(std::string* s)
	{
		char ch;
		if (!parseAlpha(&ch))
			return false;
		*s = ch;
		while (parseAlnumMinus(&ch))
			*s += ch;
		return true;
	}

	bool parseLiteral(std::string* s)
	{
		std::streampos pos = xml.tellg();
		if (!parseAssertChar('"'))
		{
			return false;
		}
		char ch;
		*s = "";
		for (;;)
		{
			if (!parseChar(&ch))
			{
				xml.seekg(pos);
				return false;
			}
			else if (ch == '>')
			{
				xml.seekg(pos);
				return false;
			}
			else if (ch == '"')
				return true;
			*s += ch;
		};
	}

	bool parseAttr(attr* attr)
	{
		std::streampos pos = xml.tellg();
		if (!parseName(&attr->name.name))
			return false;
		char ch;
		if (!parseChar(&ch))
		{
			xml.seekg(pos);
			return false;
		}
		if (ch == ':')
		{
			attr->name.nspace = attr->name.name;
			if (!parseName(&attr->name.name))
			{
				xml.seekg(pos);
				return false;
			}
			if (!parseChar(&ch))
			{
				xml.seekg(pos);
				return false;
			}
		}
		if (ch != '=')
		{
			xml.seekg(pos);
			return false;
		}
		if (!parseLiteral(&attr->value))
		{
			xml.seekg(pos);
			return false;
		}
		return true;
	}

	bool parseTag(xml_tag* tag)
	{
		if (!parseAssertChar('<'))
			return false;
		std::streampos pos = xml.tellg();
		tag->type = xml_tag::OPENING;
		if (parseAssertChar('/'))
			tag->type = xml_tag::CLOSING;
		else if (parseAssertChar('?'))
			tag->type = xml_tag::HEADER;
		if (!parseName(&tag->name.name))
		{
			xml.seekg(pos);
			return false;
		}
		if (parseAssertChar(':'))
		{
			tag->name.nspace = tag->name.name;
			if (!parseName(&tag->name.name))
			{
				xml.seekg(pos);
				return false;
			}
		}
		if (tag->type == xml_tag::HEADER || tag->type == xml_tag::OPENING)
		{
			while (parseAssertChar(' '))
			{
				attr attr;
				if (!parseAttr(&attr))
				{
					xml.seekg(pos);
					return false;
				}
				// we do not use std::set because original order of attributes is important
				if (std::find(tag->attrs.begin(), tag->attrs.end(), attr) == tag->attrs.end())
					tag->attrs.push_back(attr);
			}
		}
		if (parseAssertChar('/'))
			tag->type = xml_tag::OPENING_AND_CLOSING;
		else
			parseAssertChar('?');
		if (!parseAssertChar('>'))
		{
			xml.seekg(pos);
			return false;
		}
		return true;
	}

	bool parseUtf8Char(std::string* s)
	{
		std::streampos pos = xml.tellg();
		char ch;
		if (!parseChar(&ch))
			return false;
		int len;
		if ((ch & 0xFE) == 0xFC) // 1111110x
			len = 5;
		else if ((ch & 0xFC) == 0xF8) // 111110xx
			len = 4;
		else if ((ch & 0xF8) == 0xF0) // 11110xxx
			len = 3;
		else if ((ch & 0xF0) == 0xE0) // 1110xxxx
			len = 2;
		else if ((ch & 0xE0) == 0xC0) // 110xxxxx
			len = 1;
		else
		{
			xml.seekg(pos);
			return false;
		}
		*s = ch;
		for (int i = 0; i < len; i++)
		{
			if (!parseChar(&ch))
			{
				xml.seekg(pos);
				return false;
			}
			if ((ch & 0xC0) != 0x80) // 10xxxxxx
			{
				xml.seekg(pos);
				return false;
			}
			*s += ch;
		}
		return true;
	}

	bool parseAssertString(const std::string s)
	{
		std::streampos pos = xml.tellg();
		for (size_t i = 0; i < s.length(); i++)
			if (!parseAssertChar(s[i]))
			{
				xml.seekg(pos);
				return false;
			}
		return true;
	}

	bool parseEntity(std::string* s)
	{
		const std::string entities[] = { "&quot;", "&amp;", "&apos;", "&lt;", "&gt;" };
		for (int i = 0; i < sizeof(entities) / sizeof(entities[0]); i++)
			if (parseAssertString(entities[i]))
			{
				*s = entities[i];
				return true;
			}
		return false;
	}
};

xml_fixer::xml_fixer()
{
}

std::string xml_fixer::fix(const std::string& xml)
{
	impl().xml.str(xml);
	std::stack<xml_name> open_tags;
	std::string fixed_xml;
	for (;;)
	{
		xml_tag tag;
		std::string entity;
		std::string utf8_char;
		char ch;
		if (impl().parseTag(&tag))
		{
			if (tag.type == xml_tag::OPENING_AND_CLOSING && open_tags.empty() && impl().xml.peek() != EOF)
			{
				// some text after root opening and closing tag!
				open_tags.push(tag.name);
				tag.type = xml_tag::OPENING;
			}
			else if (tag.type == xml_tag::CLOSING)
			{
				if (!open_tags.empty() && open_tags.top() == tag.name)
					open_tags.pop();
				else
				{
					if (open_tags.empty() && impl().xml.peek() != EOF)
					{
						// some text after root closing tag!
						continue;
					}
					fixed_xml += xml_tag(xml_tag::OPENING, tag.name).toText();
				}
			}
			else if (tag.type == xml_tag::OPENING)
			{
				if (open_tags.size() == xmlParserMaxDepth)
				{
					// We are aproaching libxml security limit of tags depth.
					// Something is probably wrong, especially when we are
					// processing corrupted file.
					// Important question: what should we do now, skip
					// new tag or close some tags?
					// Current algorithm preserves as many tags as possible,
					// so we are simply closing the last tag.
					fixed_xml += xml_tag(xml_tag::CLOSING, open_tags.top()).toText();
					open_tags.pop();
				}
				open_tags.push(tag.name);
			}
			fixed_xml += tag.toText();
		}
		else if (impl().parseEntity(&entity))
			fixed_xml += entity;
		else if (impl().parseUtf8Char(&utf8_char))
			fixed_xml += utf8_char;
		else if (impl().parseChar(&ch))
		{
			if (isascii(ch) && ch != '<' && ch != '&')
				fixed_xml += ch;
		}
		else
			break;
	}
	while (!open_tags.empty())
	{
		const xml_name& tag = open_tags.top();
		fixed_xml += xml_tag(xml_tag::CLOSING, tag).toText();
		open_tags.pop();
	}
	return fixed_xml;
}

} // namespace docwire
