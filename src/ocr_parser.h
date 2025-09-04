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

#include "chain_element.h"
#include <cstdint>
#include "data_source.h"
#include <filesystem>
#include "language.h"
#include "ocr_export.h"
#include <optional>
#include "pimpl.h"
#include <vector>

namespace docwire
{

namespace ocr
{

struct PleaseWait {};

} // namespace ocr

struct ocr_confidence_threshold { std::optional<float> v; };
struct ocr_data_path { std::filesystem::path v; };
struct ocr_timeout { std::optional<int32_t> v; };

class DOCWIRE_OCR_EXPORT OCRParser : public ChainElement, public with_pimpl<OCRParser>
{
private:
    using with_pimpl<OCRParser>::impl;
    friend pimpl_impl<OCRParser>;

public:

    OCRParser(const std::vector<Language>& languages = {},
        ocr_confidence_threshold ocr_confidence_threshold_arg = {},
        ocr_timeout ocr_timeout_arg = {},
        ocr_data_path ocr_data_path_arg = {});

    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

    bool is_leaf() const override { return false; }

private:
    void parse(const data_source& data, const std::vector<Language>& languages);
};

} // namespace docwire

#endif // DOCWIRE_OCR_PARSER_H
