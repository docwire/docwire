/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TRANSCRIBE_H
#define DF0_TRANSCRIBE_H

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>
#include <string>

namespace df0
{
namespace openai
{

enum class transcribe_model
{
	gpt_4o_transcribe, gpt_4o_mini_transcribe, whisper_1
};

template<typename auditor_type = audit::null_auditor>
class DF0_OPENAI_EXPORT transcribe
    : public pipeline::transformer_element<transcribe<auditor_type>>,
      public with_pimpl<transcribe<auditor_type>>,
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
	transcribe(
		const std::string& api_key,
		transcribe_model model = transcribe_model::gpt_4o_transcribe)
		: transcribe(api_key, model, audit::auditor_ref<stateless_auditor_type>{})
	{}

	transcribe(
		const std::string& api_key,
		transcribe_model model,
		audit::auditor_ref<auditor_type> auditor);

	/**
	* @brief Executes transform operation for given node data.
	* @see df0::message_ptr
	* @param msg
	* @param emit_message
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<transcribe<auditor_type>>::impl;
};

// Deduction guide for CTAD
transcribe(const std::string&, transcribe_model) -> transcribe<audit::null_auditor>;

} // namespace openai
} // namespace df0

#endif //DF0_OPENAI_TRANSCRIBE_H
