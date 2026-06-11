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

namespace
{

constexpr std::string_view default_passage_prefix = "passage: ";
constexpr std::string_view default_query_prefix = "query: ";

std::shared_ptr<docwire::ai::ai_runner> make_default_runner()
{
    return std::make_shared<docwire::ai::ct2::ct2_runner>(
        docwire::resource_path("multilingual-e5-small-ct2-int8"));
}

} // anonymous namespace

namespace docwire::ai::local::passage
{
embedder::embedder()
    : docwire::ai::embed(make_default_runner(), std::string{default_passage_prefix})
{}
} // namespace docwire::ai::local::passage

namespace docwire::ai::local::query
{
embedder::embedder()
    : docwire::ai::embed(make_default_runner(), std::string{default_query_prefix})
{}
} // namespace docwire::ai::local::query
