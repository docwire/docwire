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
#include "parser_builder.h"
#include <string>
#include "tags.h"
#include <vector>

namespace docwire
{

class Metadata;

class DllExport PDFParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;
		attributes::Metadata metaData(const data_source& data) const;

	public:
		PDFParser();
		~PDFParser();
		void parse(const data_source& data) const override;
		inline static const std::vector<mime_type> supported_mime_types = { mime_type{"application/pdf"} };
};

} // namespace docwire

#endif
