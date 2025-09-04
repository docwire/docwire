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

#include "output.h"

#include "data_source.h"
#include "error_tags.h"
#include "ref_or_owned.h"
#include "throw_if.h"

namespace docwire
{

continuation OutputChainElement::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (msg->is<std::exception_ptr>())
		return emit_message(std::move(msg));
	if (std::holds_alternative<ref_or_owned<std::ostream>>(m_out_obj))
	{
		throw_if (!msg->is<data_source>(),
			"Only data_source elements are supported", errors::program_logic{});
		std::shared_ptr<std::istream> in_stream = msg->get<data_source>().istream();
		std::get<ref_or_owned<std::ostream>>(m_out_obj).get() << in_stream->rdbuf();
	}
	else
		std::get<ref_or_owned<std::vector<message_ptr>>>(m_out_obj).get().push_back(std::move(msg));
	return continuation::proceed;
}

} // namespace docwire
