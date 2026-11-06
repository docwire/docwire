/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "llama_models.h"
#include "resource_path.h"

namespace df0::ai::llama {
std::filesystem::path granite_model_path() {
  try {
    return resource_path(std::string(granite_relative_path));
  } catch (const std::exception &) {
    std::throw_with_nested(std::runtime_error(
        "Granite model not found. Rebuild vcpkg with feature "
        "'local-ai-model-granite' to use the default Granite model, "
        "or supply your own GGUF model_path in model_inference_config."));
  }
}
} // namespace df0::ai::llama
