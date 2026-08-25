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

#include "iconv_wrapper.h"

#include <cerrno>
#include <cstring>
#include <mutex>
#include <iconv.h>
#include "throw_if.h"
#include "error.h"

#ifdef open
#undef open
#endif

#ifdef close
#undef close
#endif

namespace docwire::detail::iconv_wrapper
{

namespace
{
std::mutex iconv_open_mutex;
} // namespace

DOCWIRE_CORE_EXPORT void open(state& st)
{
    std::lock_guard<std::mutex> lock(iconv_open_mutex);
    iconv_t descriptor = iconv_open(st.to.c_str(), st.from.c_str());
    throw_if(descriptor == (iconv_t)(-1), "iconv_open() failed", strerror(errno), st.from, st.to);
    st.descriptor = reinterpret_cast<std::uintptr_t>(descriptor);
}

DOCWIRE_CORE_EXPORT void close(state& st) noexcept
{
    if (st.descriptor == 0)
        return;
    iconv_t descriptor = reinterpret_cast<iconv_t>(st.descriptor);
    if (descriptor != (iconv_t)(-1))
        iconv_close(descriptor);
    st.descriptor = 0;
}

DOCWIRE_CORE_EXPORT std::string convert(state& st, std::string_view input)
{
    if (st.descriptor == 0)
        throw make_error("Cannot convert: iconv descriptor is not open");

    if (input.empty())
        return {};

    iconv_t descriptor = reinterpret_cast<iconv_t>(st.descriptor);
    const char* inptr = input.data();
    size_t inbytesleft = input.size();

    // Reset converter to its initial shift state.
    ::iconv(descriptor, nullptr, nullptr, nullptr, nullptr);

    size_t output_size = input.size() * 2;
    std::string output(output_size, '\0');
    size_t total_written = 0;

    while (inbytesleft > 0)
    {
        char* outptr = output.data() + total_written;
        size_t outbytesleft = output.size() - total_written;

        size_t result = ::iconv(descriptor, const_cast<char**>(&inptr), &inbytesleft, &outptr, &outbytesleft);
        total_written = output.size() - outbytesleft;

        if (result == static_cast<size_t>(-1))
        {
            if (errno == E2BIG)
            {
                output.resize(output.size() * 2);
            }
            else
            {
                throw make_error("iconv() failed", strerror(errno));
            }
        }
    }

    output.resize(total_written);
    return output;
}

} // namespace docwire::detail::iconv_wrapper
