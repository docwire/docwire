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

#ifndef DOCWIRE_AI_EMBED_H
#define DOCWIRE_AI_EMBED_H

#include "ai_export.h"
#include "ai_runner.h"
#include "pipeline/element_base.hpp"
#include "not_null.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire::ai
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_AI_EXPORT embed
    : public pipeline::transformer_element<embed<auditor_type>>,
      public with_pimpl<embed<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    /**
     * @brief Construct a local AI embed chain element with a specific model runner and prefix.
     *
     * @param model_runner The model runner to use for generating embeddings.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     * @param auditor Optional injected auditor for observability.
     */
    explicit embed(
        not_null<std::shared_ptr<ai_runner>> model_runner,
        std::string prefix,
        audit::auditor_ref<auditor_type> auditor = {});

    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
    using with_pimpl<embed<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
};

} // namespace docwire::ai

#endif // DOCWIRE_AI_EMBED_H
