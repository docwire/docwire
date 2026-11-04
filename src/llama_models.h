/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_LLAMA_MODELS_H
#define DF0_AI_LLAMA_MODELS_H
#include "ai_llama_export.h"
#include <filesystem>
#include <string_view>

namespace df0::ai::llama {
inline constexpr std::string_view granite_relative_path =
    "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf";

DF0_AI_LLAMA_EXPORT std::filesystem::path granite_model_path();
} // namespace df0::ai::llama
#endif
