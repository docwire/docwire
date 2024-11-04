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

#include "content_type_xlsb.h"

#include "zip_reader.h"

namespace docwire::content_type::xlsb
{

void detect(data_source& data)
{
    confidence xlsx_confidence = data.mime_type_confidence(mime_type { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" });
    if (xlsx_confidence < confidence { 90 })
        return;
    if (data.highest_mime_type_confidence() >= confidence { 99 })
		return;
    ZipReader unzip{data};
    try
    {
        unzip.open();
        if (unzip.exists("xl/workbook.bin"))
            data.add_mime_type(mime_type { "application/vnd.ms-excel.sheet.binary.macroenabled.12" }, confidence { 99 });
    }
    catch (const std::exception& e)
    {
        data.add_mime_type(
            mime_type { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" },
            confidence { static_cast<int8_t>(xlsx_confidence.v * 0.8) });
    }
}

} // namespace docwire::content_type::iwork
