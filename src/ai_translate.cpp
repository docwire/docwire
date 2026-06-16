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

#include "ai_translate.h"
#include "model_chain_element.h"

namespace docwire::ai
{

translate::translate(const std::string& language, std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime)
    : model_chain_element(
          "Your task is to translate every message to " + language + " language.\n\n", runner, lifetime)
{
}
} // namespace docwire::ai
