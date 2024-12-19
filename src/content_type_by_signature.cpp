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

#include "content_type_by_signature.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/split.hpp>
#include <magic.h>
#include "resource_path.h"
#include "throw_if.h"

namespace docwire::content_type::by_signature
{

void detect(data_source& data, allow_multiple allow_multiple)
{
    if (data.highest_mime_type_confidence() >= confidence::high)
		return;
    magic_t magic_cookie = magic_open(allow_multiple.v ? MAGIC_MIME_TYPE | MAGIC_CONTINUE : MAGIC_MIME_TYPE);
    throw_if (magic_cookie == NULL);
    throw_if (magic_load(magic_cookie, resource_path("libmagic/misc/magic.mgc").string().c_str()) != 0, magic_error(magic_cookie));
    size_t bytes_max;
    throw_if (magic_getparam(magic_cookie, MAGIC_PARAM_BYTES_MAX, &bytes_max) != 0, magic_error(magic_cookie));
    std::span<const std::byte> span = data.span(length_limit{bytes_max});
    const char* file_types = magic_buffer(magic_cookie, span.data(), span.size());
    throw_if (file_types == NULL, magic_error(magic_cookie));
    std::string file_types_str { file_types };
    auto splitIt = boost::make_split_iterator(file_types_str, boost::first_finder("\\012- "));
    while (splitIt != boost::split_iterator<std::string::iterator>()) 
    {
        data.add_mime_type(
            mime_type { std::string{splitIt->begin(), splitIt->end()} },
            confidence::very_high
        );
        ++splitIt;
    }
    magic_close(magic_cookie);
}

} // namespace docwire::content_type::by_signature
