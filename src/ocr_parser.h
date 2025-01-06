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

#ifndef DOCWIRE_OCR_PARSER_H
#define DOCWIRE_OCR_PARSER_H

#include <string>

#include "language.h"
#include "parser.h"
#include "pimpl.h"

namespace docwire
{

struct ocr_data_path { std::filesystem::path v; };
struct ocr_timeout { std::optional<int32_t> v; };

class DllExport OCRParser : public Parser, public with_pimpl<OCRParser>
{
private:
    using with_pimpl<OCRParser>::impl;
    friend pimpl_impl<OCRParser>;

public:

    OCRParser(const std::vector<Language>& languages = {},
        ocr_timeout ocr_timeout_arg = {}, ocr_data_path ocr_data_path_arg = {});

    void parse(const data_source& data) override;
    inline static const std::vector<mime_type> supported_mime_types =
    {
        mime_type{"image/tiff"},
        mime_type{"image/jpeg"},
        mime_type{"image/bmp"},
        mime_type{"image/x-ms-bmp"},
        mime_type{"image/png"},
        mime_type{"image/x-portable-anymap"},
        mime_type{"image/webp"}
    };

private:
    std::string parse(const data_source& data, const std::vector<Language>& languages);
};

} // namespace docwire

#endif // DOCWIRE_OCR_PARSER_H
