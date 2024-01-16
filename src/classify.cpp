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

#include "classify.h"

#include "boost/algorithm/string/join.hpp"
#include "log.h"

namespace docwire
{
namespace openai
{

Classify::Classify(const std::set<std::string>& categories, const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: Chat("Your task is to classify every message to one of the following categories: " + boost::algorithm::join(categories, ", "),
			api_key, model, temperature, image_detail)
{
	docwire_log_func_with_args(categories, temperature);
}

Classify::Classify(const Classify& other)
	: Chat(other)
{
	docwire_log_func();
}

Classify::~Classify()
{
}

Classify* Classify::clone() const
{
	return new Classify(*this);
}

} // namespace openai
} // namespace docwire
