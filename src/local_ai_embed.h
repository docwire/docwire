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

#ifndef DOCWIRE_LOCAL_AI_EMBED_H
#define DOCWIRE_LOCAL_AI_EMBED_H

#include "ai_embed.h"
#include "local_ai_export.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"
#include <concepts>

namespace docwire::ai::local::passage
{

/**
 * @brief Embeds a passage (document chunk) using the local AI model's default passage prefix.
 * The appropriate prefix for the underlying model (e.g. "passage: " for multilingual-e5-small)
 * is applied automatically. No model-specific knowledge required at the call site.
 */
template<typename auditor_type = audit::null_auditor>
class DOCWIRE_LOCAL_AI_EXPORT embedder
    : public docwire::pipeline::transformer_element<embedder<auditor_type>>
    , public docwire::ai::embed<auditor_type>
{
  public:
    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    embedder()
        : embedder(audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit embedder(audit::auditor_ref<auditor_type> auditor);
};
} // namespace docwire::ai::local::passage

namespace docwire::ai::local::query
{
/**
 * @brief Embeds a search query (search input) using the local AI model's default query prefix.
 *
 * The appropriate prefix for the underlying model (e.g. "query: " for multilingual-e5-small)
 * is applied automatically. No model-specific knowledge required at the call site.

 */
template<typename auditor_type = audit::null_auditor>
class DOCWIRE_LOCAL_AI_EXPORT embedder
    : public docwire::pipeline::transformer_element<embedder<auditor_type>>
    , public docwire::ai::embed<auditor_type>
{
  public:
    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    embedder()
        : embedder(audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit embedder(audit::auditor_ref<auditor_type> auditor);
};

} // namespace docwire::ai::local::query

// Deduction guides for CTAD
docwire::ai::local::passage::embedder() -> docwire::ai::local::passage::embedder<audit::null_auditor>;
docwire::ai::local::query::embedder() -> docwire::ai::local::query::embedder<audit::null_auditor>;

#endif // DOCWIRE_LOCAL_AI_EMBED_H
