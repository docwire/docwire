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

#include "llama_models.h"
#include "resource_path.h"

namespace docwire::ai::llama {
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
} // namespace docwire::ai::llama
