/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "local_ai_embed.h"
#include "ct2_runner.h"
#include "resource_path.h"
#include <string>

namespace docwire::ai::local
{
namespace
{

constexpr std::string_view e5_passage_prefix = "passage: ";
constexpr std::string_view e5_query_prefix = "query: ";

std::shared_ptr<docwire::ai::ai_runner> make_default_runner()
{
    return std::make_shared<docwire::ai::ct2::ct2_runner>(
        resource_path("multilingual-e5-small-ct2-int8"));
}

} // anonymous namespace

embed_passage::embed_passage()
    : docwire::ai::embed(make_default_runner(), std::string{e5_passage_prefix})
{}

embed_query::embed_query()
    : docwire::ai::embed(make_default_runner(), std::string{e5_query_prefix})
{}

embed_with_prefix::embed_with_prefix(std::string prefix)
    : docwire::ai::embed(make_default_runner(), std::move(prefix))
{}
} // namespace docwire::ai::local
