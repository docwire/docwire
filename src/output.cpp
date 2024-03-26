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

#include "exception.h"
#include <fstream>

namespace docwire
{

void
OutputChainElement::process(Info &info) const
{
	if (!std::holds_alternative<tag::File>(info.tag))
		throw LogicError("Only tag::File tags are supported by OutputChainElement chain element");
	std::shared_ptr<std::istream> in_stream = std::get<tag::File>(info.tag).access_stream();
	*m_out_stream << in_stream->rdbuf();
}

} // namespace docwire
