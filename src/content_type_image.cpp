/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_image.h"

namespace df0::content_type::image
{

void detect(data_source& data)
{
    if (data.highest_mime_type_confidence() >= confidence::highest)
        return;

    // Heuristic fallback: Only run if libmagic failed and returned octet-stream (or nothing).
    if (!data.mime_types.empty() && 
        data.mime_type_confidence(mime_type{"application/octet-stream"}) < confidence::medium)
    {
        return;
    }

    // We only need the first 16 bytes to identify these image headers
    std::string_view header = data.string_view(length_limit{16});
    if (header.size() < 2)
        return;

    // BMP: Starts with "BM"
    if (header[0] == 'B' && header[1] == 'M')
    {
        data.add_mime_type(mime_type{"image/bmp"}, confidence::highest);
        return;
    }

    // WEBP: Starts with "RIFF", followed by 4 bytes of size, followed by "WEBP"
    if (header.size() >= 12 && 
        header.substr(0, 4) == "RIFF" && 
        header.substr(8, 4) == "WEBP")
    {
        data.add_mime_type(mime_type{"image/webp"}, confidence::highest);
        return;
    }
}

} // namespace df0::content_type::image
