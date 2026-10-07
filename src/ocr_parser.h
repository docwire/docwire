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

#ifndef DOCWIRE_OCR_PARSER_H
#define DOCWIRE_OCR_PARSER_H

#include "pipeline/element_base.hpp"
#include <cstdint>
#include "data_source.h"
#include <filesystem>
#include "language.h"
#include "ocr_export.h"
#include <optional>
#include "pimpl.h"
#include <vector>
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire
{

namespace ocr
{

struct please_wait {};

} // namespace ocr

struct ocr_confidence_threshold { std::optional<float> v; };
struct ocr_data_path { std::filesystem::path v; };
struct ocr_timeout { std::optional<int32_t> v; };

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OCR_EXPORT ocr_parser
    : public pipeline::transformer_element<ocr_parser<auditor_type>>,
      public with_pimpl<ocr_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
private:
    using with_pimpl<ocr_parser<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
    friend pimpl_impl<ocr_parser<auditor_type>>;

public:

    explicit ocr_parser(const std::vector<language>& languages = {},
        ocr_confidence_threshold ocr_confidence_threshold_arg = {},
        ocr_timeout ocr_timeout_arg = {},
        ocr_data_path ocr_data_path_arg = {},
        audit::auditor_ref<auditor_type> auditor = {});

    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
    void parse(const data_source& data, const std::vector<language>& languages);
};

} // namespace docwire

#endif // DOCWIRE_OCR_PARSER_H
