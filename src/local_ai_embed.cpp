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

#include "local_ai_embed.h"
#include "ct2_runner.h"
#include "resource_path.h"
#include <string>
#include "audit/auditor_ref.hpp"
#include "audit/json_stream_auditor.hpp"

namespace
{

constexpr std::string_view default_passage_prefix = "passage: ";
constexpr std::string_view default_query_prefix = "query: ";

template<typename auditor_type>
std::shared_ptr<df0::ai::ai_runner> make_default_runner(
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::ct2::ct2_runner<auditor_type>>(
        df0::resource_path("multilingual-e5-small-ct2-int8"),
        auditor);
}

} // anonymous namespace

namespace df0::ai::local::passage
{
template<typename auditor_type>
embedder<auditor_type>::embedder(df0::audit::auditor_ref<auditor_type> auditor)
    : df0::ai::embed<auditor_type>(
          make_default_runner(auditor),
          std::string{default_passage_prefix},
          auditor)
{}
} // namespace df0::ai::local::passage

namespace df0::ai::local::query
{
template<typename auditor_type>
embedder<auditor_type>::embedder(df0::audit::auditor_ref<auditor_type> auditor)
    : df0::ai::embed<auditor_type>(
          make_default_runner(auditor),
          std::string{default_query_prefix},
          auditor)
{}
} // namespace df0::ai::local::query

template class DOCWIRE_LOCAL_AI_EXPORT df0::ai::local::passage::embedder<df0::audit::null_auditor>;
template class DOCWIRE_LOCAL_AI_EXPORT df0::ai::local::passage::embedder<df0::audit::json_stream_auditor>;

template class DOCWIRE_LOCAL_AI_EXPORT df0::ai::local::query::embedder<df0::audit::null_auditor>;
template class DOCWIRE_LOCAL_AI_EXPORT df0::ai::local::query::embedder<df0::audit::json_stream_auditor>;
