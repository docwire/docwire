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

namespace docwire
{
namespace openai
{

/**
 * @brief OpenAI models available for chat.
 */
enum class Model
{
	/// GPT-5: The best model for coding and agentic tasks across domains
	gpt_5,
	/// GPT-5 mini: A faster, cost-efficient version of GPT-5 for well-defined tasks
	gpt_5_mini,
	/// GPT-5 nano: Fastest, most cost-efficient version of GPT-5
	gpt_5_nano,
	/// GPT-5 Chat: GPT-5 model used in ChatGPT
	gpt_5_chat_latest,
	/// GPT-4.1: Fast, highly intelligent model with largest context window
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
	/// o3-deep-research: OpenAI most powerful deep research model
	o3_deep_research,
	/// o3-mini: A small model alternative to o3
	o3_mini,
	/// o4-mini: Faster, more affordable reasoning model
	o4_mini,
	/// o4-mini-deep-research: Faster, more affordable deep research model
	o4_mini_deep_research
};

enum class ImageDetail
{
	low, high, automatic
};

class DOCWIRE_OPENAI_EXPORT Chat : public ChainElement, public with_pimpl<Chat>
{
public:
	Chat(const std::string& system_message, const std::string& api_key, Model model = Model::gpt_5, float temperature = 0.7, ImageDetail image_detail = ImageDetail::automatic);

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::message_ptr
	* @see docwire::message_callbacks
	* @param msg The input message to process.
	* @param emit_message Callback invoked to emit produced messages.
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

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
