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

#include "charset_converter.h"

#include <cstring>
#include <iconv.h>
#include <mutex>
#include "throw_if.h"

namespace docwire
{

template<>
struct pimpl_impl<charset_converter> : pimpl_impl_base
{
	iconv_t descriptor;
};

namespace
{
	std::mutex iconv_mutex;	
} // anonymous namespace

	charset_converter::charset_converter(const std::string &from, const std::string &to)
	{
		std::lock_guard<std::mutex> mutex_lock(iconv_mutex);
		impl().descriptor = iconv_open(to.c_str(), from.c_str());
		throw_if(impl().descriptor == (iconv_t)(-1), "iconv_open() failed", strerror(errno), from, to);
	}
	
	charset_converter::~charset_converter()
	{
		std::lock_guard<std::mutex> mutex_lock(iconv_mutex);
		iconv_close(impl().descriptor);
	}
	
	std::string charset_converter::convert(const std::string &input)
	{	
		size_t output_max_size = 2 * input.length();
		std::unique_ptr<char[]> output{new char[output_max_size]};

		const char* inbuf = input.c_str();
		size_t inbytesleft = input.length();
		char* outbuf = output.get();
		size_t outbytesleft = output_max_size;
	
		std::lock_guard<std::mutex> mutex_lock(iconv_mutex);
		for(;;)
		{
			size_t result = iconv(impl().descriptor, const_cast<char**>(&inbuf), &inbytesleft, &outbuf, &outbytesleft);
			if (result == 0) break;
			throw_if(result == (size_t)-1 && errno == E2BIG, "iconv() failed", strerror(errno));
			inbuf++; inbytesleft--;
		}

		return std::string{output.get(), output_max_size - outbytesleft};
	}

} // namespace docwire
