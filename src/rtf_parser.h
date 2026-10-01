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

#ifndef DOCWIRE_RTF_PARSER_H
#define DOCWIRE_RTF_PARSER_H

#include "pipeline/element_base.hpp"
#include "rtf_export.h"

namespace docwire
{

class DOCWIRE_RTF_EXPORT rtf_parser : public pipeline::transformer_element<rtf_parser>
{
	public:
		rtf_parser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

} // namespace docwire

#endif
