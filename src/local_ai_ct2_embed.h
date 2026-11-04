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

#ifndef DF0_LOCAL_AI_CT2_EMBED_H
#define DF0_LOCAL_AI_CT2_EMBED_H

#include "ai_ct2_export.h"
#include "ai_embed.h"
#include "local_ai_ct2_runner_factory.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"
#include <concepts>

namespace {

constexpr std::string_view default_passage_prefix = "passage: ";
constexpr std::string_view default_query_prefix = "query: ";

} // anonymous namespace

namespace df0::ai::local::ct2::passage
{

/**
 * @brief Embeds a passage (document chunk) using the default local CT2 embedding model.
 *
 * The appropriate prefix for the underlying model, `"passage: "` for
 * multilingual-e5-small, is applied automatically. No model-specific
 * knowledge is required at the call site.
 */
template<typename auditor_type = df0::audit::null_auditor>
class embedder
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
        : embedder(df0::audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit embedder(
        df0::audit::auditor_ref<auditor_type> auditor)
        : df0::ai::embed<auditor_type>(
              make_embedding_runner(auditor),
              std::string{default_passage_prefix},
              auditor)
    {}
};

// Deduction guide for CTAD
embedder() -> embedder<df0::audit::null_auditor>;

} // namespace df0::ai::local::ct2::passage

namespace df0::ai::local::ct2::query
{

/**
 * @brief Embeds a search query using the default local CT2 embedding model.
 *
 * The appropriate prefix for the underlying model, `"query: "` for
 * multilingual-e5-small, is applied automatically. No model-specific
 * knowledge is required at the call site.
 */
template<typename auditor_type = df0::audit::null_auditor>
class embedder
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
        : embedder(df0::audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit embedder(
        df0::audit::auditor_ref<auditor_type> auditor)
        : df0::ai::embed<auditor_type>(
              make_embedding_runner(auditor),
              std::string{default_query_prefix},
              auditor)
    {}
};

// Deduction guide for CTAD
embedder() -> embedder<df0::audit::null_auditor>;

} // namespace df0::ai::local::ct2::query

#endif // DF0_LOCAL_AI_CT2_EMBED_H
