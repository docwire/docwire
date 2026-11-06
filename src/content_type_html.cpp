/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_html.h"

namespace df0::content_type::html
{

void detect(data_source& data)
{
    // NOTE: libmagic >= 5.47 correctly identifies HTML/XHTML natively, so we must allow those alongside text/xml and application/xml
    if (!data.mime_types.empty() && 
        data.mime_type_confidence(mime_type { "text/xml" }) < confidence::medium &&
        data.mime_type_confidence(mime_type { "application/xml" }) < confidence::medium &&
        data.mime_type_confidence(mime_type { "text/html" }) < confidence::medium &&
        data.mime_type_confidence(mime_type { "application/xhtml+xml" }) < confidence::medium)
      return;
    std::string_view initial_xml = data.string_view(length_limit{1024});
    if (initial_xml.find("<html") != std::string_view::npos || initial_xml.find("<HTML") != std::string_view::npos)
		data.add_mime_type(mime_type { "text/html" }, confidence::highest);
}

} // namespace df0::content_type::html
