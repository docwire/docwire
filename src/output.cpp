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

#include <fstream>

namespace docwire
{

void
OutputChainElement::process(Info &info) const
{
	throw_if (!std::holds_alternative<data_source>(info.tag), "Only data_source tags are supported");
	std::shared_ptr<std::istream> in_stream = std::get<data_source>(info.tag).istream();
	*m_out_stream << in_stream->rdbuf();
}

} // namespace docwire
