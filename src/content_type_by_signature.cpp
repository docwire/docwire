/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_by_signature.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/split.hpp>
#include "error_tags.h"
#include <filesystem>
#include <magic.h>
#include "resource_path.h"
#include "throw_if.h"

namespace df0
{

template<>
struct pimpl_impl<content_type::by_signature::database> : public pimpl_impl_base
{
    pimpl_impl()
        : magic_cookie(magic_open(MAGIC_NONE))
    {
        throw_if (magic_cookie == nullptr);

        try
        {
            const std::filesystem::path main_db_path = resource_path("libmagic/misc/magic.mgc");
            const std::string magic_db_path = main_db_path.string();
            throw_if (magic_load(magic_cookie, magic_db_path.c_str()) != 0, magic_error(magic_cookie));
            throw_if (magic_getparam(magic_cookie, MAGIC_PARAM_BYTES_MAX, &bytes_max) != 0, magic_error(magic_cookie));
        } catch (const std::exception&) {
            std::throw_with_nested(make_error("Failed to initialize content type signatures database", errors::program_corrupted{}));
        }
    }
    ~pimpl_impl() { magic_close(magic_cookie); }
    content_type::by_signature::allow_multiple allow_multiple;
    magic_t magic_cookie;
    size_t bytes_max;
};

} // namespace df0

namespace df0::content_type::by_signature
{

database::database() = default;

void detect(data_source& data, const database& database_to_use, allow_multiple allow_multiple)
{
    if (data.highest_mime_type_confidence() >= confidence::high)
		return;
    magic_setflags(database_to_use.impl().magic_cookie,
        allow_multiple.v ? MAGIC_MIME_TYPE | MAGIC_CONTINUE : MAGIC_MIME_TYPE);
    std::span<const std::byte> span = data.span(length_limit{database_to_use.impl().bytes_max});
    const char* file_types = magic_buffer(database_to_use.impl().magic_cookie, span.data(), span.size());
    throw_if (file_types == NULL, magic_error(database_to_use.impl().magic_cookie));
    std::string file_types_str { file_types };
    auto splitIt = boost::make_split_iterator(file_types_str, boost::first_finder("\\012- "));
    while (splitIt != boost::split_iterator<std::string::iterator>()) 
    {
        std::string mt_str = std::string{splitIt->begin(), splitIt->end()};
        
        // Normalize libmagic quirks to standard MIME types
        if (mt_str == "text/xml") mt_str = "application/xml";
        else if (mt_str == "text/rtf") mt_str = "application/rtf";
        else if (mt_str == "image/x-ms-bmp") mt_str = "image/bmp";

        data.add_mime_type(
            mime_type { mt_str },
            confidence::very_high
        );
        ++splitIt;
    }
}

} // namespace df0::content_type::by_signature
