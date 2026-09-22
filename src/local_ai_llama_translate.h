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

#ifndef DOCWIRE_LOCAL_AI_LLAMA_TRANSLATE_H
#define DOCWIRE_LOCAL_AI_LLAMA_TRANSLATE_H

#include "ai_translate.h"
#include "local_ai_llama_runner_factory.h"
#include "model_inference_config.h"
#include <filesystem>

namespace docwire::ai::local::llama {

class translate : public docwire::ai::translate {
public:
#ifdef DOCWIRE_GRANITE
  // Constructor with Default Granite model
  explicit translate(
      const std::string &language,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::translate(language, make_default_runner(), lifetime){};
#endif
  // Constructor allowing custom model path but with default config
  explicit translate(
      const std::string &language, const std::filesystem::path &model_path,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::translate(language, make_runner(model_path), lifetime){};
  // Constructor allowing custom model and relevant config
  explicit translate(
      const std::string &language, docwire::ai::model_inference_config config,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::translate(language, make_runner(config), lifetime){};
};

} // namespace docwire::ai::local::llama

#endif // DOCWIRE_LOCAL_AI_LLAMA_TRANSLATE_H
