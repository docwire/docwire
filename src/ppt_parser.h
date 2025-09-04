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

#ifndef DOCWIRE_PPT_PARSER_H
#define DOCWIRE_PPT_PARSER_H

#include "ole_office_formats_export.h"
#include "chain_element.h"

namespace docwire
{

class ThreadSafeOLEStorage;

class DOCWIRE_OLE_OFFICE_FORMATS_EXPORT PPTParser : public ChainElement
{
	public:
		PPTParser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
