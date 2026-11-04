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

#ifndef DOCWIRE_LOCAL_AI_LLAMA_RUNNER_FACTORY_H
#define DOCWIRE_LOCAL_AI_LLAMA_RUNNER_FACTORY_H

#include "ai_runner.h"
#include "llama_runner.h"
#include "llama_models.h" // IWYU pragma: export
#include "model_inference_config.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"

#include <concepts>
#include <filesystem>
#include <memory>
#include <utility>

namespace df0::ai::local::llama
{

inline model_inference_config default_config(std::filesystem::path model_path)
{
    model_inference_config config;
    config.model_path = std::move(model_path);
    config.max_tokens = df0::ai::token_limit{256};
    config.n_ctx = df0::ai::context_size{4096};
    config.n_threads = df0::ai::thread_count{4};
    config.temp = df0::ai::temperature{0.2f};
    config.min_probability = df0::ai::min_p{0.05f};
    return config;
}

#ifdef DOCWIRE_GRANITE
template<typename auditor_type>
inline std::shared_ptr<df0::ai::ai_runner> make_default_runner(
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::llama::llama_runner<auditor_type>>(
        default_config(df0::ai::llama::granite_model_path()),
        auditor);
}

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
inline std::shared_ptr<df0::ai::ai_runner> make_default_runner()
{
    return make_default_runner<stateless_auditor_type>(
        df0::audit::auditor_ref<stateless_auditor_type>{});
}
#endif

template<typename auditor_type>
inline std::shared_ptr<df0::ai::ai_runner> make_runner(
    const std::filesystem::path& model_path,
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::llama::llama_runner<auditor_type>>(
        default_config(std::filesystem::path{model_path}),
        auditor);
}

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
inline std::shared_ptr<df0::ai::ai_runner> make_runner(
    const std::filesystem::path& model_path)
{
    return make_runner<stateless_auditor_type>(
        model_path, df0::audit::auditor_ref<stateless_auditor_type>{});
}

template<typename auditor_type>
inline std::shared_ptr<df0::ai::ai_runner> make_runner(
    model_inference_config config,
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::llama::llama_runner<auditor_type>>(
        std::move(config),
        auditor);
}

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
inline std::shared_ptr<df0::ai::ai_runner> make_runner(
    model_inference_config config)
{
    return make_runner<stateless_auditor_type>(
        std::move(config), df0::audit::auditor_ref<stateless_auditor_type>{});
}

} // namespace df0::ai::local::llama

#endif
