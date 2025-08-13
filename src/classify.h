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

#ifndef DOCWIRE_OPENAI_CLASSIFY_H
#define DOCWIRE_OPENAI_CLASSIFY_H

#include "chat.h"
#include <set>

namespace docwire
{
namespace openai
{

class DOCWIRE_OPENAI_EXPORT Classify : public Chat
{
public:
	Classify(const std::set<std::string>& categories, const std::string& api_key, Model model = Model::gpt_5, float temperature = 0, ImageDetail image_detail = ImageDetail::automatic);
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_CLASSIFY_H
