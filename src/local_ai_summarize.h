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

#ifndef DOCWIRE_LOCAL_AI_SUMMARIZE_H
#define DOCWIRE_LOCAL_AI_SUMMARIZE_H

#include "local_ai_export.h"
#include "ai_summarize.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"

namespace docwire::ai::local
{

template<typename auditor_type = docwire::audit::null_auditor>
class DOCWIRE_LOCAL_AI_EXPORT summarize
    : public docwire::pipeline::transformer_element<summarize<auditor_type>>
    , public docwire::ai::summarize
{
public:
    explicit summarize(
        model_lifetime_policy lifetime = model_lifetime_policy::persistent,
        docwire::audit::auditor_ref<auditor_type> auditor = {});

    explicit summarize(
        std::shared_ptr<docwire::ai::ai_runner> runner,
        model_lifetime_policy lifetime = model_lifetime_policy::persistent);
};

summarize() -> summarize<docwire::audit::null_auditor>;
summarize(model_lifetime_policy) -> summarize<docwire::audit::null_auditor>;
summarize(std::shared_ptr<docwire::ai::ai_runner>, model_lifetime_policy)
    -> summarize<docwire::audit::null_auditor>;

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_SUMMARIZE_H
