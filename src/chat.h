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

#ifndef DOCWIRE_OPENAI_CHAT_H
#define DOCWIRE_OPENAI_CHAT_H

#include "chain_element.h"
#include "exception.h"

namespace docwire
{
namespace openai
{

enum class Model
{
	gpt35_turbo, gpt35_turbo_16k, gpt35_turbo_1106,
	gpt4, gpt4_32k, gpt4_1106_preview
};

class DllExport Chat : public ChainElement
{
public:
	Chat(const std::string& system_message, const std::string& api_key, Model model = Model::gpt35_turbo, float temperature = 0.7);
	Chat(const Chat& other);
	virtual ~Chat();

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Info
	* @param info
	*/
	void process(Info &info) const;

	bool is_leaf() const override
	{
		return false;
	}

	/**
	* @brief Creates clone of the Chat
	* @return new Chat
	*/
	Chat* clone() const override;

	DOCWIRE_EXCEPTION_DEFINE(HttpError, RuntimeError);
	DOCWIRE_EXCEPTION_DEFINE(ParseResponseError, RuntimeError);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_CHAT_H
