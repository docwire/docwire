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

#include "content_type_iwork.h"

#include "zip_reader.h"

namespace docwire::content_type::iwork
{

void detect(data_source& data)
{
    confidence zip_confidence = data.mime_type_confidence(mime_type { "application/zip" });
    if (zip_confidence < confidence::medium)
        return;
    if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    ZipReader unzip{data};
    try
    {
        unzip.open();
        if (unzip.exists("presentation.apxl"))
            data.add_mime_type(mime_type { "application/vnd.apple.keynote" }, confidence::highest);
        else if (unzip.exists("index.xml") || unzip.exists("index.apxl"))
        {
            std::string contents;
            if (unzip.exists("index.xml"))
                unzip.read("index.xml", &contents);
            else
                unzip.read("index.apxl", &contents);
            if (contents.find("<sl:document"))
                data.add_mime_type(mime_type { "application/vnd.apple.pages" }, confidence::highest);
            else if (contents.find("<ls:document"))
                data.add_mime_type(mime_type { "application/vnd.apple.numbers" }, confidence::highest);
            else if (contents.find("<key:presentation"))
                data.add_mime_type(mime_type { "application/vnd.apple.keynote" }, confidence::highest);
            else
            {
                data.add_mime_type(mime_type { "application/vnd.apple.pages" }, confidence::low);
                data.add_mime_type(mime_type { "application/vnd.apple.numbers" }, confidence::low);
                data.add_mime_type(mime_type { "application/vnd.apple.keynote" }, confidence::low);
            }
        }
    }
    catch (const std::exception& e)
    {
        data.add_mime_type(
            mime_type { "application/zip" },
            confidence::low);
    }
}

} // namespace docwire::content_type::iwork
