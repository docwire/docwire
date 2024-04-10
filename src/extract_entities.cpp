/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "extract_entities.h"

#include "log.h"

namespace docwire
{
namespace openai
{

ExtractEntities::ExtractEntities(const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: Chat("Your task is to find all dates, timespans, addresses, companies and person names inside every message. Show it as formatted list.", api_key, model, temperature, image_detail)
{
	docwire_log_func();
}

ExtractEntities::ExtractEntities(const ExtractEntities& other)
	: Chat(other)
{
	docwire_log_func();
}

ExtractEntities::~ExtractEntities()
{
}

} // namespace openai
} // namespace docwire
