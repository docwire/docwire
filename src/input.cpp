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

#include "input.h"

#include "parsing_chain.h"
#include "log.h"
#include "log_data_source.h" // IWYU pragma: keep

using namespace docwire;

continuation InputChainElement::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  docwire_log_func();
	if (msg->is<pipeline::start_processing>())
	{
		docwire_log_var(m_data.get());
		return emit_message(std::move(m_data.get()));
	}
	return emit_message(std::move(msg));
}
