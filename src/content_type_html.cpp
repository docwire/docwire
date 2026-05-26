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

#include "content_type_html.h"

namespace docwire::content_type::html
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

} // namespace docwire::content_type::html
