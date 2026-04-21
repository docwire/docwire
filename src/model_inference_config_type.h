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

#ifndef DOCWIRE_AI_MODEL_INFERENCE_CONFIG_TYPE_H
#define DOCWIRE_AI_MODEL_INFERENCE_CONFIG_TYPE_H

#include "strong_type.h"
#include <cstddef>

namespace docwire::ai
{
struct context_size_tag
{
};
struct thread_count_tag
{
};
struct token_limit_tag
{
};
struct temperature_tag
{
};
struct min_p_tag
{
};
struct batch_size_tag
{
};

using batch_size = strong_type<std::size_t, batch_size_tag>;
using context_size = strong_type<std::size_t, context_size_tag>;
using thread_count = strong_type<std::size_t, thread_count_tag>;
using token_limit = strong_type<std::size_t, token_limit_tag>;

using temperature = strong_type<float, temperature_tag>;
using min_p = strong_type<float, min_p_tag>;
} // namespace docwire::ai

#endif
