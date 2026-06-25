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

#ifndef DOCWIRE_OPENAI_TEXT_TO_SPEECH_H
#define DOCWIRE_OPENAI_TEXT_TO_SPEECH_H

#include "chain_element.h"
#include "openai_export.h"
#include "message.h"
#include <string>

namespace docwire
{
namespace openai
{

class DOCWIRE_OPENAI_EXPORT text_to_speech : public chain_element, public with_pimpl<text_to_speech>
{
public:
	enum class model
	{
		gpt_4o_mini_tts, tts_1, tts_1_hd
	};

	enum class voice
	{
		alloy, echo, fable, onyx, nova, shimmer
	};

	text_to_speech(const std::string& api_key, model model = model::gpt_4o_mini_tts, voice voice = voice::alloy);

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::message_ptr, docwire::message_callbacks
	* @param msg
	* @param emit_message
	**/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	using with_pimpl<text_to_speech>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_TEXT_TO_SPEECH_H
