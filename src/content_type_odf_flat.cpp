/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_odf_flat.h"

namespace df0::content_type::odf_flat
{

void detect(data_source& data)
{
    if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    if (!data.mime_types.empty() && 
        data.mime_type_confidence(mime_type { "text/xml" }) < confidence::medium &&
        data.mime_type_confidence(mime_type { "application/xml" }) < confidence::medium)
        return;
    std::string_view initial_xml = data.string_view(length_limit{4096});
    if (initial_xml.find("office:document") != std::string_view::npos)
    {
        if (initial_xml.find("application/vnd.oasis.opendocument.text") != std::string_view::npos)
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.text-flat-xml" }, confidence::highest);
        else if (initial_xml.find("application/vnd.oasis.opendocument.spreadsheet") != std::string_view::npos)
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.spreadsheet-flat-xml" }, confidence::highest);
        else if (initial_xml.find("application/vnd.oasis.opendocument.presentation") != std::string_view::npos)
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.presentation-flat-xml" }, confidence::highest);
        else if (initial_xml.find("application/vnd.oasis.opendocument.graphics") != std::string_view::npos)
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.graphics-flat-xml" }, confidence::highest);
    }
}

} // namespace df0::content_type::odf_flat
