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

#ifndef DOCWIRE_EML_PARSER_H
#define DOCWIRE_EML_PARSER_H

#include "mail_export.h"

#include "pipeline/element_base.hpp"
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_MAIL_EXPORT eml_parser : public pipeline::transformer_element<eml_parser>, public with_pimpl<eml_parser>
{
	private:
		using with_pimpl<eml_parser>::impl;
		friend pimpl_impl<eml_parser>;

	public:
		eml_parser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

} // namespace docwire

#endif
