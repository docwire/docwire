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

#ifndef DOCWIRE_OPENAI_DETECT_SENTIMENT_H
#define DOCWIRE_OPENAI_DETECT_SENTIMENT_H

#include "chat.h"

namespace docwire
{
namespace openai
{

class DllExport DetectSentiment : public Chat
{
public:
	DetectSentiment(const std::string& api_key, Model model = Model::gpt35_turbo, float temperature = 0);
	DetectSentiment(const DetectSentiment& other);
	virtual ~DetectSentiment();

	/**
	* @brief Creates clone of the DetectSentiment
	* @return new DetectSentiment
	*/
	DetectSentiment* clone() const override;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_DETECT_SENTIMENT_H
