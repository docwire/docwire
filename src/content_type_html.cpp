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

#include "content_type_html.h"

namespace docwire::content_type::html
{

void detect(data_source& data)
{
    if (data.mime_type_confidence(mime_type { "text/xml" }) < confidence::medium)
        return;
    if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    std::string initial_xml = data.string(length_limit{1024});
    if (initial_xml.find("<html") != std::string::npos || initial_xml.find("<HTML") != std::string::npos)
		data.add_mime_type(mime_type { "text/html" }, confidence::highest);
}

} // namespace docwire::content_type::html
