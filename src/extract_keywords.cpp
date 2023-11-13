/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "extract_keywords.h"

#include "log.h"

namespace docwire
{
namespace openai
{

ExtractKeywords::ExtractKeywords(unsigned int max_keywords, const std::string& api_key, float temperature)
	: Chat("Your task is to identify and extract " +
			std::to_string(max_keywords) +
			" most important keywords or key phrases from every message. The goal is to capture the most relevant and significant terms within the text.",
			api_key, temperature)
{
	docwire_log_func_with_args(max_keywords, temperature);
}

ExtractKeywords::ExtractKeywords(const ExtractKeywords& other)
	: Chat(other)
{
	docwire_log_func();
}

ExtractKeywords::~ExtractKeywords()
{
}

ExtractKeywords* ExtractKeywords::clone() const
{
	return new ExtractKeywords(*this);
}

} // namespace openai
} // namespace docwire
