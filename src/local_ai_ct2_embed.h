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

#ifndef DOCWIRE_LOCAL_AI_CT2_EMBED_H
#define DOCWIRE_LOCAL_AI_CT2_EMBED_H

#include "ai_ct2_export.h"
#include "ai_embed.h"
#include "local_ai_ct2_runner_factory.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"

namespace {

constexpr std::string_view default_passage_prefix = "passage: ";
constexpr std::string_view default_query_prefix = "query: ";

} // anonymous namespace

namespace docwire::ai::local::ct2::passage
{

/**
 * @brief Embeds a passage (document chunk) using the default local CT2 embedding model.
 *
 * The appropriate prefix for the underlying model, `"passage: "` for
 * multilingual-e5-small, is applied automatically. No model-specific
 * knowledge is required at the call site.
 */
template<typename auditor_type = docwire::audit::null_auditor>
class DOCWIRE_AI_CT2_EXPORT embedder
    : public docwire::pipeline::transformer_element<embedder<auditor_type>>
    , public docwire::ai::embed<auditor_type>
{
public:
    explicit embedder(
        docwire::audit::auditor_ref<auditor_type> auditor = {})
        : docwire::ai::embed<auditor_type>(
              make_embedding_runner(auditor),
              std::string{default_passage_prefix},
              auditor)
    {}
};

} // namespace docwire::ai::local::ct2::passage

namespace docwire::ai::local::ct2::query
{

/**
 * @brief Embeds a search query using the default local CT2 embedding model.
 *
 * The appropriate prefix for the underlying model, `"query: "` for
 * multilingual-e5-small, is applied automatically. No model-specific
 * knowledge is required at the call site.
 */
template<typename auditor_type = docwire::audit::null_auditor>
class DOCWIRE_AI_CT2_EXPORT embedder
    : public docwire::pipeline::transformer_element<embedder<auditor_type>>
    , public docwire::ai::embed<auditor_type>
{
public:
    explicit embedder(
        docwire::audit::auditor_ref<auditor_type> auditor = {})
        : docwire::ai::embed<auditor_type>(
              make_embedding_runner(auditor),
              std::string{default_query_prefix},
              auditor)
    {}
};

} // namespace docwire::ai::local::ct2::query

#endif // DOCWIRE_LOCAL_AI_CT2_EMBED_H
