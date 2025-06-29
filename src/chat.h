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
#include "openai_export.h"
#include "tags.h"

namespace docwire
{
namespace openai
{

/**
 * @brief OpenAI models available for chat.
 */
enum class Model
{
	/// ChatGPT-4o: GPT-4o model used in ChatGPT
	chatgpt_4o_latest,
	/// GPT-4.1: Flagship GPT model for complex tasks
	gpt_41,
	/// GPT-4.1 mini: Balanced for intelligence, speed, and cost
	gpt_41_mini,
	/// GPT-4.1 nano: Fastest, most cost-effective GPT-4.1 model
	gpt_41_nano,
	/// GPT-4o: Fast, intelligent, flexible GPT model
	gpt_4o,
	/// GPT-4o mini: Fast, affordable small model for focused tasks
	gpt_4o_mini,
	/// o3: OpenAI most powerful reasoning model
	o3,
	/// o3-pro: Version of o3 with more compute for better responses
	o3_pro,
	/// o3-mini: A small model alternative to o3
	o3_mini,
	/// o4-mini: Faster, more affordable reasoning model
	o4_mini,
};

enum class ImageDetail
{
	low, high, automatic
};

class DOCWIRE_OPENAI_EXPORT Chat : public ChainElement, public with_pimpl<Chat>
{
public:
	Chat(const std::string& system_message, const std::string& api_key, Model model = Model::gpt_4o, float temperature = 0.7, ImageDetail image_detail = ImageDetail::automatic);

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Tag
	* @param tag
	* @param callback
	*/
	continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;

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
