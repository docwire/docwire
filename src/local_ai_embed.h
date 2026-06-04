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

#ifndef DOCWIRE_LOCAL_AI_EMBED_H
#define DOCWIRE_LOCAL_AI_EMBED_H

#include "ai_embed.h"
#include "local_ai_export.h"

namespace docwire::ai::local::passage
{

/**
 * @brief Embeds a passage (document chunk) using the local AI model's default passage prefix.
 * The appropriate prefix for the underlying model (e.g. "passage: " for multilingual-e5-small)
 * is applied automatically. No model-specific knowledge required at the call site.
 */
class DOCWIRE_LOCAL_AI_EXPORT embedder : public docwire::ai::embed
{
  public:
    embedder();
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
class DOCWIRE_LOCAL_AI_EXPORT embedder : public docwire::ai::embed
{
  public:
    embedder();
};

} // namespace docwire::ai::local::query

#endif // DOCWIRE_LOCAL_AI_EMBED_H
