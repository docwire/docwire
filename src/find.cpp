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

#include "find.h"

#include "log_scope.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace docwire
{
namespace openai
{

find::find(const std::string& what, const std::string& api_key, model model, float temperature, image_detail image_detail)
	: chat("Your task is to find all occurrences of \"" + what + "\" in the provided text or image.\n\n"
	       "1. On the first line, write only the total count of occurrences found. If none are found, write 0.\n"
	       "2. On each subsequent line, provide a brief, one-sentence description of where each occurrence is located. For example: 'A red car is parked near the building on the left.' or 'The word \"processing\" appears in the first paragraph.'\n\n"
	       "Do not describe the entire image or text. Focus only on the location of what you were asked to find.",
	       api_key, model, temperature, image_detail)
{
	log_scope(what, model, temperature, image_detail);
}

} // namespace openai
} // namespace docwire
