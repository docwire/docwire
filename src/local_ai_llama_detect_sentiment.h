/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H
#define DF0_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H

#include "ai_detect_sentiment.h"
#include "model_inference_config.h"
#include "local_ai_llama_runner_factory.h"
#include <filesystem>

namespace df0::ai::local::llama {

class detect_sentiment :
	public df0::pipeline::transformer_element<detect_sentiment>,
	public df0::ai::detect_sentiment {
public:
#ifdef DF0_GRANITE
  // Constructor with Default Granite model
  explicit detect_sentiment(
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::detect_sentiment(make_default_runner(), lifetime){};
#endif
  // Constructor allowing custom model path but with default config
  explicit detect_sentiment(
      const std::filesystem::path &model_path,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::detect_sentiment(make_runner(model_path), lifetime){};
  // Constructor allowing custom model and relevant config
  explicit detect_sentiment(
      df0::ai::model_inference_config config,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::detect_sentiment(make_runner(config), lifetime){};
};

} // namespace df0::ai::local::llama

#endif // DF0_LOCAL_AI_LLAMA_DETECT_SENTIMENT_H
