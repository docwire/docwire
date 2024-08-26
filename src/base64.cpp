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

#include "base64.h"

#include <libbase64.h>

namespace docwire::base64
{

std::string encode(std::span<const std::byte> input_data)
{
	// max size of output is 4 * (ceil(n / 3))
	// ceil(x / y) can be written as (x + y - 1) / y
	size_t max_out_size = 4 * ((input_data.size() + 3 - 1) / 3);
	std::string out(max_out_size, '\0');
	size_t out_size = 0;
	base64_encode(reinterpret_cast<const char*>(input_data.data()), input_data.size(), out.data(), &out_size, 0);
	out.resize(out_size);
	return out;
}

} // namespace docwire::base64
