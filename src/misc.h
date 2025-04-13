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

#ifndef DOCWIRE_MISC_H
#define DOCWIRE_MISC_H

#include "core_export.h"
#include "data_source.h"
#include <string>
#include <vector>
#include "wv2/src/ustring.h"
#include <time.h>

struct tm;

namespace docwire
{

using namespace wvWare;

typedef std::vector<std::string> svector;

DOCWIRE_CORE_EXPORT std::string formatTable(std::vector<svector>& mcols);
DOCWIRE_CORE_EXPORT std::string formatUrl(const std::string& mlink_url, const std::string& mlink_text);
DOCWIRE_CORE_EXPORT std::string formatList(std::vector<std::string>& mlist);
DOCWIRE_CORE_EXPORT std::string formatNumberedList(std::vector<std::string>& mlist);

DOCWIRE_CORE_EXPORT std::string ustring_to_string(const UString& s);

/**
	Parses date and time in %Y-%m-%dT%H:%M:%S or %Y%m%d;%H%M%S format
**/
DOCWIRE_CORE_EXPORT bool string_to_date(const std::string& s, tm& date);

DOCWIRE_CORE_EXPORT std::string date_to_string(const tm& date);

DOCWIRE_CORE_EXPORT std::string uint_to_string(unsigned int value);

DOCWIRE_CORE_EXPORT UString utf8_to_ustring(const std::string& src);

DOCWIRE_CORE_EXPORT std::string unichar_to_utf8(unsigned int unichar);

/**
	UTF16 characters take from 2 to 4 bytes length.
	Code points from 0x0000 to 0xFFFF require two bytes (so called BMP, most popular characters).
	But there are rare used characters which use codes between 0x10000 to 0x10FFFF.
	In that case utf16 character requires 4 bytes. Algorithm:

	0x10000 is subtracted from the code point, leaving a 20 bit number in the range 0..0xFFFFF.
	The top ten bits (a number in the range 0..0x3FF) are added to 0xD800 to give the first code
	unit or lead surrogate, which will be in the range 0xD800..0xDBFF (previous versions of the
	Unicode Standard referred to these as high surrogates). The low ten bits (also in the range 0..0x3FF)
	are added to 0xDC00 to give the second code unit or trail surrogate, which will be in the range
	0xDC00..0xDFFF (previous versions of the Unicode Standard referred to these as low surrogates).

	In BMP set, there are no 16-bit characters which first 6 bits have 110110 or 110111. They are reserved.

	In the following function below, we check if first two bytes require another two bytes.
**/
inline bool utf16_unichar_has_4_bytes(unsigned int ch)
{
	return (ch & 0xFC00) == 0xD800;
}

DOCWIRE_CORE_EXPORT std::string int_to_str(int i);

DOCWIRE_CORE_EXPORT int str_to_int(const std::string& s);

DOCWIRE_CORE_EXPORT bool is_encrypted_with_ms_offcrypto(const data_source& data);

DOCWIRE_CORE_EXPORT tm *thread_safe_gmtime (const time_t *timer, struct tm &time_buffer);

} // namespace docwire

#endif
