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

#include "charset_converter.h"

#include <cstring>
#include <mutex>
#include <iconv.h>
#include "throw_if.h"

namespace docwire
{

template<>
struct pimpl_impl<charset_converter> : pimpl_impl_base
{
	struct iconv_descriptor
	{
		iconv_t descriptor;

		// The glibc implementation of iconv_open is not entirely thread-safe.
		// It can race on its internal cache of gconv modules. To prevent this,
		// we must serialize all calls to iconv_open across all threads.
		// This mutex is global and static to ensure that only one thread
		// can be inside iconv_open at any given time. The performance impact
		// is minimal as this lock is only taken once per thread during the
		// first-time initialization of a charset_converter.
		static std::mutex iconv_open_mutex;

		iconv_descriptor(const std::string& from, const std::string& to)
		{
			std::lock_guard<std::mutex> lock(iconv_open_mutex);
			descriptor = iconv_open(to.c_str(), from.c_str());
			throw_if(descriptor == (iconv_t)(-1), "iconv_open() failed", strerror(errno), from, to);
		}

		~iconv_descriptor()
		{
			if (descriptor != (iconv_t)(-1))
				iconv_close(descriptor);
		}

		// iconv_t is a raw C handle, so copying or moving it without proper semantics is unsafe.
		iconv_descriptor(const iconv_descriptor&) = delete;
		iconv_descriptor& operator=(const iconv_descriptor&) = delete;
		iconv_descriptor(iconv_descriptor&&) = delete;
		iconv_descriptor& operator=(iconv_descriptor&&) = delete;
	};

	pimpl_impl(const std::string& from, const std::string& to)
		: m_descriptor(from, to)
	{}

	iconv_descriptor m_descriptor;
};

std::mutex pimpl_impl<charset_converter>::iconv_descriptor::iconv_open_mutex;

charset_converter::charset_converter(const std::string &from, const std::string &to)
	: with_pimpl<charset_converter>(from, to)
{
}

charset_converter::~charset_converter() = default;

std::string charset_converter::convert(std::string_view input) const
{	
	if (input.empty())
		return "";

	// iconv API is not const-correct for the input buffer.
	const char* inptr = input.data();
	size_t inbytesleft = input.length();

	iconv_t descriptor = impl().m_descriptor.descriptor;
	// Reset descriptor to its initial state for a new conversion.
	iconv(descriptor, nullptr, nullptr, nullptr, nullptr);

	// A reasonable starting point for most conversions. UTF-8 can take up to 4 bytes per character.
	size_t output_size = input.length() * 2;
	std::string output(output_size, '\0');
	size_t total_written = 0;

	while (inbytesleft > 0)
	{
		char* outptr = output.data() + total_written;
		size_t outbytesleft = output.size() - total_written;

		size_t result = iconv(descriptor, const_cast<char**>(&inptr), &inbytesleft, &outptr, &outbytesleft);
		total_written = output.size() - outbytesleft;

		if (result == (size_t)-1)
		{
			if (errno == E2BIG) // Output buffer is full.
			{
				// Double the buffer size and continue.
				output.resize(output.size() * 2);
			}
			else // A non-recoverable error occurred.
				throw make_error("iconv() failed", strerror(errno));
		}
	}
	output.resize(total_written);
	return output;
}

} // namespace docwire
