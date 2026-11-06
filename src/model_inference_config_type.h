/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_MODEL_INFERENCE_CONFIG_TYPE_H
#define DF0_AI_MODEL_INFERENCE_CONFIG_TYPE_H

#include "strong_type.h"
#include <cstddef>

namespace df0::ai
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
} // namespace df0::ai

#endif
