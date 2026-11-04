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

#ifndef DF0_LOCAL_AI_LLAMA_FIND_H
#define DF0_LOCAL_AI_LLAMA_FIND_H

#include "ai_find.h"
#include "local_ai_llama_runner_factory.h"
#include "model_inference_config.h"
#include <filesystem>

namespace df0::ai::local::llama {

class find :
	public df0::pipeline::transformer_element<find>,
	public df0::ai::find {
public:
#ifdef DF0_GRANITE
  // Constructor with Default Granite model
  explicit find(
      const std::string &what,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::find(what, make_default_runner(), lifetime){};
#endif
  // Constructor allowing custom model path but with default config
  explicit find(
      const std::string &what, const std::filesystem::path &model_path,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::find(what, make_runner(model_path), lifetime){};
  // Constructor allowing custom model and relevant config
  explicit find(
      const std::string &what, df0::ai::model_inference_config config,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::find(what, make_runner(config), lifetime){};
};

} // namespace df0::ai::local::llama

#endif // DF0_LOCAL_AI_LLAMA_FIND_H
