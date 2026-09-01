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

#ifndef DOCWIRE_CHARSET_CONVERTER_H
#define DOCWIRE_CHARSET_CONVERTER_H

#include <string>
#include <string_view>
#include <utility>
#include "core_export.h"
#include "iconv_wrapper.h"

namespace docwire
{

class charset_converter
{
public:
	charset_converter(std::string from, std::string to)
		: m_state{0, std::move(from), std::move(to)}
	{
		detail::iconv_wrapper::open(m_state);
	}

	~charset_converter()
	{
		detail::iconv_wrapper::close(m_state);
	}

	std::string convert(std::string_view input) const
	{
		return detail::iconv_wrapper::convert(const_cast<detail::iconv_wrapper::state&>(m_state), input);
	}

private:
	detail::iconv_wrapper::state m_state;
};

} // namespace docwire

#endif
