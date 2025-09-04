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

#ifndef DOCWIRE_EML_PARSER_H
#define DOCWIRE_EML_PARSER_H

#include "mail_export.h"

#include "chain_element.h"
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_MAIL_EXPORT EMLParser : public ChainElement, public with_pimpl<EMLParser>
{
	private:
		using with_pimpl<EMLParser>::impl;
		friend pimpl_impl<EMLParser>;

	public:
		EMLParser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
