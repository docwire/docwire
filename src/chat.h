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

namespace docwire
{
namespace openai
{

enum class Model
{
	gpt35_turbo, gpt35_turbo_0125, gpt35_turbo_1106,
	gpt4, gpt4_0613,
	gpt4_32k, gpt4_32k_0613,
	gpt4_turbo_preview, gpt4_0125_preview, gpt4_1106_preview,
	gpt4_vision_preview, gpt4_1106_vision_preview
};

enum class ImageDetail
{
	low, high, automatic
};

class DllExport Chat : public ChainElement, public with_pimpl<Chat>
{
public:
	Chat(const std::string& system_message, const std::string& api_key, Model model = Model::gpt35_turbo, float temperature = 0.7, ImageDetail image_detail = ImageDetail::automatic);
	Chat(Chat&& other);
	virtual ~Chat();

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Info
	* @param info
	*/
	void process(Info& info) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	using with_pimpl<Chat>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_CHAT_H
