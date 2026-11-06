/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_iwork.h"

#include "zip_reader.h"

namespace df0::content_type::iwork
{

void detect(data_source& data)
{
    if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    if (!data.mime_types.empty())
    {
        confidence zip_confidence = data.mime_type_confidence(mime_type { "application/zip" });
        if (zip_confidence < confidence::medium)
            return;
    }
    zip_reader unzip{data};
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
            if (contents.find("<sl:document") != std::string::npos)
                data.add_mime_type(mime_type { "application/vnd.apple.pages" }, confidence::highest);
            else if (contents.find("<ls:document") != std::string::npos)
                data.add_mime_type(mime_type { "application/vnd.apple.numbers" }, confidence::highest);
            else if (contents.find("<key:presentation") != std::string::npos)
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

} // namespace df0::content_type::iwork
