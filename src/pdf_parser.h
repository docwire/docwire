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

#include "parser.h"
#include "pdf_export.h"
#include "pimpl.h"
#include "tags.h"
#include <vector>

namespace docwire
{

class Metadata;

class DOCWIRE_PDF_EXPORT PDFParser : public Parser, public with_pimpl<PDFParser>
{
	private:
		using with_pimpl<PDFParser>::impl;
		using with_pimpl<PDFParser>::renew_impl;
		using with_pimpl<PDFParser>::destroy_impl;
		friend pimpl_impl<PDFParser>;
		attributes::Metadata metaData(const data_source& data);

	public:
		PDFParser();
		PDFParser(PDFParser&&) = default;
		~PDFParser();
		void parse(const data_source& data) override;
		const std::vector<mime_type> supported_mime_types() override
		{
			return { mime_type{"application/pdf"} };
		}
};

} // namespace docwire

#endif
