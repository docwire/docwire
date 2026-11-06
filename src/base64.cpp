/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "base64.h"

#include "error_tags.h"
#include <libbase64.h>
#include "throw_if.h"

namespace df0::base64
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

std::vector<std::byte> decode(std::string_view input_data)
{
	size_t max_out_size = (input_data.size() * 3) / 4 + 2;
	std::vector<std::byte> out(max_out_size);
	size_t out_size = 0;
	const int result = base64_decode(input_data.data(), input_data.size(), reinterpret_cast<char*>(out.data()), &out_size, 0);
	throw_if(result != 1, "Invalid base64 input data", errors::uninterpretable_data{});
	out.resize(out_size);
	return out;
}

} // namespace df0::base64
