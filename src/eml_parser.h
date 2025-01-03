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

#include <vector>

#include "parser.h"
#include "pimpl.h"

namespace docwire
{

class Metadata;
//class Attachment;

class DllExport EMLParser : public Parser, public with_pimpl<EMLParser>
{
	private:
		using with_pimpl<EMLParser>::impl;
		friend pimpl_impl<EMLParser>;

	public:
		EMLParser();
		void parse(const data_source& data) override;
		inline static const std::vector<mime_type> supported_mime_types = { mime_type{"message/rfc822"} };
};

} // namespace docwire

#endif
