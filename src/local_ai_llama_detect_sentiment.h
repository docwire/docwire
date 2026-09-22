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

#ifndef DOCWIRE_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H
#define DOCWIRE_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H

#include "ai_detect_sentiment.h"
#include "model_inference_config.h"
#include "local_ai_llama_runner_factory.h"
#include <filesystem>

namespace docwire::ai::local::llama {

class detect_sentiment : public docwire::ai::detect_sentiment {
public:
#ifdef DOCWIRE_GRANITE
  // Constructor with Default Granite model
  explicit detect_sentiment(
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::detect_sentiment(make_default_runner(), lifetime){};
#endif
  // Constructor allowing custom model path but with default config
  explicit detect_sentiment(
      const std::filesystem::path &model_path,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::detect_sentiment(make_runner(model_path), lifetime){};
  // Constructor allowing custom model and relevant config
  explicit detect_sentiment(
      docwire::ai::model_inference_config config,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::detect_sentiment(make_runner(config), lifetime){};
};

} // namespace docwire::ai::local::llama

#endif // DOCWIRE_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H
