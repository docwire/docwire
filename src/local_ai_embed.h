/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_EMBED_H
#define DF0_LOCAL_AI_EMBED_H

#include "ai_embed.h"
#include "local_ai_export.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"
#include <concepts>

namespace df0::ai::local::passage
{

/**
 * @brief Embeds a passage (document chunk) using the local AI model's default passage prefix.
 * The appropriate prefix for the underlying model (e.g. "passage: " for multilingual-e5-small)
 * is applied automatically. No model-specific knowledge required at the call site.
 */
template<typename auditor_type = audit::null_auditor>
class DF0_LOCAL_AI_EXPORT embedder
    : public df0::pipeline::transformer_element<embedder<auditor_type>>
    , public df0::ai::embed<auditor_type>
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

// Deduction guide for CTAD
embedder() -> embedder<audit::null_auditor>;

} // namespace df0::ai::local::passage

namespace df0::ai::local::query
{
/**
 * @brief Embeds a search query (search input) using the local AI model's default query prefix.
 *
 * The appropriate prefix for the underlying model (e.g. "query: " for multilingual-e5-small)
 * is applied automatically. No model-specific knowledge required at the call site.

 */
template<typename auditor_type = audit::null_auditor>
class DF0_LOCAL_AI_EXPORT embedder
    : public df0::pipeline::transformer_element<embedder<auditor_type>>
    , public df0::ai::embed<auditor_type>
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

// Deduction guide for CTAD
embedder() -> embedder<audit::null_auditor>;

} // namespace df0::ai::local::query

#endif // DF0_LOCAL_AI_EMBED_H
