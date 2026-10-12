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
#include <filesystem>
#include <memory>
#include <utility>

namespace docwire::ai::local::llama {

inline model_inference_config default_config(std::filesystem::path model_path) {
  model_inference_config config;
  config.model_path = std::move(model_path);
  config.max_tokens = docwire::ai::token_limit{256};
  config.n_ctx = docwire::ai::context_size{4096};
  config.n_threads = docwire::ai::thread_count{4};
  config.temp = docwire::ai::temperature{0.2f};
  config.min_probability = docwire::ai::min_p{0.05f};
  return config;
}

#ifdef DOCWIRE_GRANITE
inline std::shared_ptr<docwire::ai::ai_runner> make_default_runner() {
  return std::make_shared<docwire::ai::llama::llama_runner>(
      default_config(docwire::ai::llama::granite_model_path()));
}
#endif

inline std::shared_ptr<docwire::ai::ai_runner>
make_runner(const std::filesystem::path &model_path) {
  return std::make_shared<docwire::ai::llama::llama_runner>(
      default_config(model_path));
}

inline std::shared_ptr<docwire::ai::ai_runner>
make_runner(model_inference_config config) {
  return std::make_shared<docwire::ai::llama::llama_runner>(std::move(config));
}

} // namespace docwire::ai::local::llama

#endif // DOCWIRE_LOCAL_AI_LLAMA_RUNNER_FACTORY_H
