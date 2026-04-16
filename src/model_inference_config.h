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

#ifndef DOCWIRE_LOCAL_AI_MODEL_INFERENCE_CONFIG_H
#define DOCWIRE_LOCAL_AI_MODEL_INFERENCE_CONFIG_H
#include "model_inference_config_type.h"
#include <string>

namespace docwire::local_ai
{
/*
 * @brief Handles configuration for llama model initialization and paramters
 */
struct model_inference_config
{
    std::string model_path;
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

} // namespace docwire::local_ai

#endif
