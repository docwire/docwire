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

#include "summarize.h"

#include "log_scope.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace docwire
{
namespace openai
{

Summarize::Summarize(const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: Chat("Your task is to summarize every message", api_key, model, temperature, image_detail)
{
	log_scope(model, temperature, image_detail);
}

} // namespace openai
} // namespace docwire
