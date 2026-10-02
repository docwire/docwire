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

#ifndef DOCWIRE_AI_LLAMA_MODELS_H
#define DOCWIRE_AI_LLAMA_MODELS_H
#include "ai_llama_export.h"
#include <filesystem>
#include <string_view>

namespace docwire::ai::llama {
inline constexpr std::string_view granite_relative_path =
    "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf";

DOCWIRE_AI_LLAMA_EXPORT std::filesystem::path granite_model_path();
} // namespace docwire::ai::llama
#endif
