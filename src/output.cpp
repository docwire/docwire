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

#include "error_tags.h"
#include "throw_if.h"

namespace docwire
{

void
OutputChainElement::process(Info& info)
{
	if (std::holds_alternative<std::exception_ptr>(info.tag))
	{
		emit(info);
		return;
	}
	throw_if (!std::holds_alternative<data_source>(info.tag),
		"Only data_source tags are supported", errors::program_logic{});
	std::shared_ptr<std::istream> in_stream = std::get<data_source>(info.tag).istream();
	m_out_stream.get() << in_stream->rdbuf();
}

} // namespace docwire
