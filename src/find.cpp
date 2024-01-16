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

#include "find.h"

#include "log.h"

namespace docwire
{
namespace openai
{

Find::Find(const std::string& what, const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: Chat("Search for every phrase \"" + what + "\" in provided text. Partial matches are accepted. If image is provided instead of text, match objects or events as well. Start answer with number of matches (in digits) or 0 if not found and then describe what you found and where exactly matches are located.", api_key, model, temperature, image_detail)
{
	docwire_log_func_with_args(what);
}

Find::Find(const Find& other)
	: Chat(other)
{
	docwire_log_func();
}

Find::~Find()
{
}

Find* Find::clone() const
{
	return new Find(*this);
}

} // namespace openai
} // namespace docwire
