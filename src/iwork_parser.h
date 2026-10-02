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

#ifndef DOCWIRE_IWORK_PARSER_H
#define DOCWIRE_IWORK_PARSER_H

#include "iwork_export.h"
#include "pipeline/element_base.hpp"
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_IWORK_EXPORT iwork_parser : public pipeline::transformer_element<iwork_parser>, public with_pimpl<iwork_parser>
{
	public:
		iwork_parser();

		continuation operator()(message_ptr msg, const message_callbacks& emit_message);

	private:
		using with_pimpl<iwork_parser>::impl;
};

} // namespace docwire

#endif
