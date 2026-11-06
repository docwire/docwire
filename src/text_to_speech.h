/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_OPENAI_TEXT_TO_SPEECH_H
#define DF0_OPENAI_TEXT_TO_SPEECH_H

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "message.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>
#include <string>

namespace df0
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
class DF0_OPENAI_EXPORT text_to_speech
    : public pipeline::transformer_element<text_to_speech<auditor_type>>,
      public with_pimpl<text_to_speech<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	/**
	 * @brief Constructs the component with the default stateless auditing policy.
	 *
	 * This overload is available only when the configured auditor is stateless
	 * and therefore default-constructible.
	 *
	 * @tparam stateless_auditor_type Injected auditor type selected by this overload.
	 */
	template<audit::stateless_auditor stateless_auditor_type = auditor_type>
	text_to_speech(
		const std::string& api_key,
		text_to_speech_model model = text_to_speech_model::gpt_4o_mini_tts,
		text_to_speech_voice voice = text_to_speech_voice::alloy)
		: text_to_speech(api_key, model, voice,
		                 audit::auditor_ref<stateless_auditor_type>{})
	{}

	text_to_speech(
		const std::string& api_key,
		text_to_speech_model model,
		text_to_speech_voice voice,
		audit::auditor_ref<auditor_type> auditor);

	/**
	* @brief Executes transform operation for given node data.
	* @see df0::message_ptr, df0::message_callbacks
	* @param msg
	* @param emit_message
	**/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<text_to_speech<auditor_type>>::impl;
};

// Deduction guide for CTAD
text_to_speech(const std::string&, text_to_speech_model, text_to_speech_voice) -> text_to_speech<audit::null_auditor>;

} // namespace openai
} // namespace df0

#endif //DF0_OPENAI_TEXT_TO_SPEECH_H
