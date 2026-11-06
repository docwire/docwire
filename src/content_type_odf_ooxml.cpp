/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_odf_ooxml.h"

#include "zip_reader.h"

namespace df0::content_type::odf_ooxml
{

void detect(data_source& data)
{
    if (data.highest_mime_type_confidence() >= confidence::highest)
        return;

    if (!data.mime_types.empty() && data.mime_type_confidence(mime_type{"application/zip"}) < confidence::medium)
        return;

    // STEP 1: Fast Heuristic Check
    // ZIP files store their definitive index (the Central Directory) at the very END of the file.
    // libmagic >= 5.47 fails to detect OOXML/ODF from memory buffers because it cannot seek to the end.
    // To avoid downloading entire files on network streams, we first check the Local File Headers 
    // located at the beginning of the file (first 4KB).
    std::string_view initial_content = data.string_view(length_limit{4096});

    if (initial_content.find("mimetypeapplication/vnd.oasis.opendocument.text") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.text"}, confidence::highest); return;
    } else if (initial_content.find("mimetypeapplication/vnd.oasis.opendocument.spreadsheet") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.spreadsheet"}, confidence::highest); return;
    } else if (initial_content.find("mimetypeapplication/vnd.oasis.opendocument.presentation") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.presentation"}, confidence::highest); return;
    } else if (initial_content.find("mimetypeapplication/vnd.oasis.opendocument.graphics") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.graphics"}, confidence::highest); return;
    } else if (initial_content.find("word/document.xml") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"}, confidence::highest); return;
    } else if (initial_content.find("xl/workbook.xml") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}, confidence::highest); return;
    } else if (initial_content.find("ppt/presentation.xml") != std::string_view::npos) {
        data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.presentationml.presentation"}, confidence::highest); return;
    }

    // STEP 2: Deep Inspection Fallback
    // If the identifying files were not in the first 4KB (e.g., the ZIP was packed in a non-standard order),
    // we prioritize quality over speed and parse the actual ZIP Central Directory.
    zip_reader unzip{data};
    try
    {
        unzip.open();
        
        // Check OOXML
        if (unzip.exists("word/document.xml"))
            data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"}, confidence::highest);
        else if (unzip.exists("xl/workbook.xml"))
            data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}, confidence::highest);
        else if (unzip.exists("ppt/presentation.xml"))
            data.add_mime_type(mime_type{"application/vnd.openxmlformats-officedocument.presentationml.presentation"}, confidence::highest);
        
        // Check ODF
        else if (unzip.exists("mimetype"))
        {
            std::string mimetype_content;
            unzip.read("mimetype", &mimetype_content);
            if (mimetype_content.find("application/vnd.oasis.opendocument.text") != std::string::npos)
                data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.text"}, confidence::highest);
            else if (mimetype_content.find("application/vnd.oasis.opendocument.spreadsheet") != std::string::npos)
                data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.spreadsheet"}, confidence::highest);
            else if (mimetype_content.find("application/vnd.oasis.opendocument.presentation") != std::string::npos)
                data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.presentation"}, confidence::highest);
            else if (mimetype_content.find("application/vnd.oasis.opendocument.graphics") != std::string::npos)
                data.add_mime_type(mime_type{"application/vnd.oasis.opendocument.graphics"}, confidence::highest);
        }
    }
    catch (const std::exception&)
    {
        // Not a valid ZIP or failed to read, leave as is.
    }
}

} // namespace df0::content_type::odf_ooxml
