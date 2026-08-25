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

#ifndef DOCWIRE_DETAIL_USTRING_CONVERSIONS_H
#define DOCWIRE_DETAIL_USTRING_CONVERSIONS_H

#include <string>
#include "log_scope.h"
#include "wv2/src/ustring.h"

namespace docwire
{

using wvWare::UString;
using wvWare::UChar;

inline std::string unichar_to_utf8(unsigned int unichar)
{
	DOCWIRE_LOG_SCOPE(unichar);
	char out[4];
	if (unichar < 0x80)
	{
		out[0] = (char)unichar;
		return std::string(out, 1);
	}
	else if (unichar < 0x800)
	{
		out[0] = 0xC0 | (unichar >> 6);
		out[1] = 0x80 | (unichar & 0x0000003F);
		return std::string(out, 2);
	}
	else if (unichar < 0x10000)
	{
		out[0] = 0xE0 | (unichar >> 12);
		out[1] = 0x80 | ((unichar & 0x00000FFF) >> 6);
		out[2] = 0x80 | (unichar & 0x0000003F);
		return std::string(out, 3);
	}
	else
	{
		unsigned short second_byte = unichar & 0x000003FF;
		unichar = (unichar & 0x03FF0000) >> 6;
		unichar = (unichar | second_byte) + 0x10000;
		out[0] = 0xF0 | (unichar >> 18);
		out[1] = 0x80 | ((unichar & 0x0003FFFF) >> 12);
		out[2] = 0x80 | ((unichar & 0x00000FFF) >> 6);
		out[3] = 0x80 | (unichar & 0x0000003F);
		return std::string(out, 4);
	}
	return "";
}

inline bool utf16_unichar_has_4_bytes(unsigned int ch)
{
	return (ch & 0xFC00) == 0xD800;
}

inline std::string ustring_to_string(const UString& s)
{
	DOCWIRE_LOG_SCOPE();
	const UChar* src = s.data();
	int length = s.length();
	std::string r;
	int index = 0;
	while (index < length)
	{
		unsigned int ch = src[index++].unicode();
		if (utf16_unichar_has_4_bytes(ch) && index < length)
			ch = (ch << 16) + src[index++].unicode();
		if (ch != 0)
			r += unichar_to_utf8(ch);
	}
	return r;
}

inline UString utf8_to_ustring(const std::string& src)
{
	DOCWIRE_LOG_SCOPE(src);
	UString res;
	const char* str = src.data();
	size_t str_size = src.length();
	unsigned int ch;
	unsigned char tmp;
	size_t index = 0;
	res = "";
	while (index < str_size)
	{
		tmp = str[index++];
		if (tmp < 0x80)
			res += UString(tmp);
		else
		{
			ch = tmp & 0x1F;
			int len_char = 1;
			while (index < str_size)
			{
				tmp = str[index++];
				if ((tmp & 0xC0) == 0x80)
				{
					ch = ch << 6;
					ch = ch | (tmp & 0x3F);
					++len_char;
				}
				else
				{
					if (len_char < 4)
					{
						res += UString((UChar)((unsigned short)ch));
					}
					else
					{
						unsigned short first = 0xD800, second = 0xDC00;
						ch = ch & 0x001FFFFF;
						ch -= 0x10000;
						res += UString((UChar)(first | ((ch & 0x000FFC00) >> 10)));
						res += UString(((UChar)(second | (ch & 0x000003FF))));
					}
					--index;
					break;
				}
			}
		}
	}
	return res;
}

} // namespace docwire

#endif // DOCWIRE_DETAIL_USTRING_CONVERSIONS_H
