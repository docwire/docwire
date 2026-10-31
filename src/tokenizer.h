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

#ifndef DOCWIRE_AI_TOKENIZER_H
#define DOCWIRE_AI_TOKENIZER_H

#include "ai_ct2_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace docwire::ai::ct2
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_AI_CT2_EXPORT tokenizer
    : public with_pimpl<tokenizer<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    explicit tokenizer(
        const std::filesystem::path& model_data_path,
        audit::auditor_ref<auditor_type> auditor = {});

    std::vector<std::string> tokenize(const std::string& input);

    std::vector<int> encode(const std::string& input);

    std::string detokenize(const std::vector<std::string>& output_tokens);

private:
    using with_pimpl<tokenizer<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
};

} // namespace docwire::ai::ct2

#endif // DOCWIRE_AI_TOKENIZER_H
