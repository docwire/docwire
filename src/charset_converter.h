/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CHARSET_CONVERTER_H
#define DF0_CHARSET_CONVERTER_H

#include <string>
#include <string_view>
#include <utility>
#include "core_export.h"
#include "iconv_wrapper.h"

namespace df0
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

} // namespace df0

#endif
