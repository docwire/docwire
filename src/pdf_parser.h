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

#ifndef DOCWIRE_PDF_PARSER_H
#define DOCWIRE_PDF_PARSER_H

#include "chain_element.h"
#include "pdf_export.h"
#include "pimpl.h"
#include "tags.h"
#include <vector>

namespace docwire
{
class DOCWIRE_PDF_EXPORT PDFParser : public ChainElement, public with_pimpl<PDFParser>
{
	private:
		using with_pimpl<PDFParser>::impl;
		friend pimpl_impl<PDFParser>;

	public:
		PDFParser();
		continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
