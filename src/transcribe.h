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

#ifndef DOCWIRE_TRANSCRIBE_H
#define DOCWIRE_TRANSCRIBE_H

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <string>

namespace docwire
{
namespace openai
{

enum class transcribe_model
{
	gpt_4o_transcribe, gpt_4o_mini_transcribe, whisper_1
};

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OPENAI_EXPORT transcribe
    : public pipeline::transformer_element<transcribe<auditor_type>>,
      public with_pimpl<transcribe<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	transcribe(
		const std::string& api_key,
		transcribe_model model = transcribe_model::gpt_4o_transcribe,
		audit::auditor_ref<auditor_type> auditor = {});

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::message_ptr
	* @param msg
	* @param emit_message
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<transcribe<auditor_type>>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_TRANSCRIBE_H
