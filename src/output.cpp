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
#include "ref_or_owned.h"
#include "throw_if.h"

namespace docwire
{

continuation OutputChainElement::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
	if (std::holds_alternative<std::exception_ptr>(tag))
		return emit_tag(std::move(tag));
	if (std::holds_alternative<ref_or_owned<std::ostream>>(m_out_obj))
	{
		throw_if (!std::holds_alternative<data_source>(tag),
			"Only data_source tags are supported", errors::program_logic{});
		std::shared_ptr<std::istream> in_stream = std::get<data_source>(tag).istream();
		std::get<ref_or_owned<std::ostream>>(m_out_obj).get() << in_stream->rdbuf();
	}
	else
		std::get<ref_or_owned<std::vector<Tag>>>(m_out_obj).get().push_back(std::move(tag));
	return continuation::proceed;
}

} // namespace docwire
