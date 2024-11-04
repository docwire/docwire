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

#include "content_type_odf_flat.h"

namespace docwire::content_type::odf_flat
{

void detect(data_source& data)
{
    if (data.mime_type_confidence(mime_type { "text/xml" }) < confidence { 90 })
        return;
    if (data.highest_mime_type_confidence() >= confidence { 99 })
		return;
    std::string initial_xml = data.string(length_limit{1024});
    if (initial_xml.find("office:document") != std::string::npos)
    {
        if (initial_xml.find("application/vnd.oasis.opendocument.text"))
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.text-flat-xml" }, confidence { 99 });
        else if (initial_xml.find("application/vnd.oasis.opendocument.spreadsheet"))
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.spreadsheet-flat-xml" }, confidence { 99 });
        else if (initial_xml.find("application/vnd.oasis.opendocument.presentation"))
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.presentation-flat-xml" }, confidence { 99 });
        else if (initial_xml.find("application/vnd.oasis.opendocument.graphics"))
            data.add_mime_type(mime_type { "application/vnd.oasis.opendocument.graphics-flat-xml" }, confidence { 99 });
    }
}

} // namespace docwire::content_type::odf_flat
