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

#include "input.h"

#include "parsing_chain.h"
#include "log_entry.h"
#include "log_scope.h"
#include "serialization_data_source.h" // IWYU pragma: keep

using namespace docwire;

continuation input_chain_element::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log_scope();
	if (msg->is<pipeline::start_processing>())
	{
		log_entry(m_data.get());
		return emit_message(std::move(m_data.get()));
	}
	return emit_message(std::move(msg));
}
