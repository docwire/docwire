/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_MODEL_INFERENCE_CONFIG_H
#define DF0_AI_MODEL_INFERENCE_CONFIG_H
#include "model_inference_config_type.h"
#include <string>
#include <filesystem>

namespace df0::ai
{
/*
 * @brief Handles configuration for llama model initialization and paramters
 */
struct model_inference_config
{
    std::filesystem::path model_path;
    context_size n_ctx{4096};
    batch_size n_batch{1024};
    thread_count n_threads{4};
    token_limit max_tokens{512};
    temperature temp{0.2f};
    min_p min_probability{0.05f};
    bool verbose = false;
    std::string system_prompt = "Do NOT repeat the input. Answer concisely and directly.";
    std::string grammar{};
    std::string grammar_root = "root";
};

} // namespace df0::ai

#endif
