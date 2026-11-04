/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_LLAMA_TRANSLATE_H
#define DF0_LOCAL_AI_LLAMA_TRANSLATE_H

#include "ai_translate.h"
#include "local_ai_llama_runner_factory.h"
#include "model_inference_config.h"
#include <filesystem>

namespace df0::ai::local::llama {

class translate :
	public df0::pipeline::transformer_element<translate>,
	public df0::ai::translate {
public:
#ifdef DF0_GRANITE
  // Constructor with Default Granite model
  explicit translate(
      const std::string &language,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::translate(language, make_default_runner(), lifetime){};
#endif
  // Constructor allowing custom model path but with default config
  explicit translate(
      const std::string &language, const std::filesystem::path &model_path,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::translate(language, make_runner(model_path), lifetime){};
  // Constructor allowing custom model and relevant config
  explicit translate(
      const std::string &language, df0::ai::model_inference_config config,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::translate(language, make_runner(config), lifetime){};
};

} // namespace df0::ai::local::llama

#endif // DF0_LOCAL_AI_LLAMA_TRANSLATE_H
