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

#include "model_chain_element.h"

#include "data_source.h"
#include "error_tags.h"
#include "resource_path.h"
#include "throw_if.h"

namespace docwire::local_ai
{

model_chain_element::model_chain_element(const std::string& prompt)
	: docwire::local_ai::model_chain_element(prompt, std::make_shared<model_runner>(resource_path("flan-t5-large-ct2-int8")))
{}

continuation model_chain_element::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	const data_source& data = msg->get<data_source>();
	throw_if (!data.has_highest_confidence_mime_type_in({mime_type{"text/plain"}}), errors::program_logic{});
	std::string input = m_prompt + "\n" + data.string();
	std::string output = m_model_runner->process(input);

	return emit_message(data_source{std::move(output)});
}

} // namespace docwire::local_ai
