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

#include "content_type_xlsb.h"

#include "zip_reader.h"

namespace docwire::content_type::xlsb
{

void detect(data_source& data)
{
    if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    if (!data.mime_types.empty())
    {
        // NOTE: libmagic < 5.47 often reported xlsb as xlsx.
        // libmagic >= 5.47 (when given a buffer) reports it as a generic zip.
        confidence xlsx_confidence = data.mime_type_confidence(mime_type { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" });
        confidence zip_confidence = data.mime_type_confidence(mime_type { "application/zip" });
        if (xlsx_confidence < confidence::medium && zip_confidence < confidence::medium)
            return;
    }

    // STEP 1: Fast Heuristic Check
    // Check Local File Headers at the beginning of the file to avoid full downloads on network streams.
    std::string_view initial_content = data.string_view(length_limit{4096});
    if (initial_content.find("xl/workbook.bin") != std::string_view::npos)
    {
        data.add_mime_type(mime_type { "application/vnd.ms-excel.sheet.binary.macroenabled.12" }, confidence::highest);
        return;
    }

    // STEP 2: Deep Inspection Fallback
    // Parse the ZIP Central Directory at the end of the file if the heuristic failed.
    zip_reader unzip{data};
    try
    {
        unzip.open();
        if (unzip.exists("xl/workbook.bin"))
            data.add_mime_type(mime_type { "application/vnd.ms-excel.sheet.binary.macroenabled.12" }, confidence::highest);
    }
    catch (const std::exception& e)
    {
        // If it was previously detected as xlsx but failed ZIP parsing, downgrade it.
        if (data.mime_type_confidence(mime_type { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" }) >= confidence::medium)
        {
            data.add_mime_type(
                mime_type { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" },
                confidence::low);
        }
    }
}

} // namespace docwire::content_type::xlsb
