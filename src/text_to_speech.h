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

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "message.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <string>

namespace docwire
{
namespace openai
{

enum class text_to_speech_model
{
	gpt_4o_mini_tts, tts_1, tts_1_hd
};

enum class text_to_speech_voice
{
	alloy, echo, fable, onyx, nova, shimmer
};

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OPENAI_EXPORT text_to_speech
    : public pipeline::transformer_element<text_to_speech<auditor_type>>,
      public with_pimpl<text_to_speech<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	text_to_speech(
		const std::string& api_key,
		text_to_speech_model model = text_to_speech_model::gpt_4o_mini_tts,
		text_to_speech_voice voice = text_to_speech_voice::alloy,
		audit::auditor_ref<auditor_type> auditor = {});

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::message_ptr, docwire::message_callbacks
	* @param msg
	* @param emit_message
	**/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<text_to_speech<auditor_type>>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_TEXT_TO_SPEECH_H
