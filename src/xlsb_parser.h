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

#ifndef DOCWIRE_XLSB_PARSER_H
#define DOCWIRE_XLSB_PARSER_H

#include "chain_element.h"
#include "message.h"
#include "xlsb_export.h"

namespace docwire
{

class ZipReader;

class DOCWIRE_XLSB_EXPORT XLSBParser : public ChainElement, public with_pimpl<XLSBParser>
{
	private:
		friend pimpl_impl<XLSBParser>;
		using with_pimpl<XLSBParser>::impl;

	public:
		XLSBParser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
