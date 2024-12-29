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

#include "parser.h"
#include "tags.h"
#include <vector>

namespace docwire
{

class ZipReader;

class DllExport XLSBParser : public Parser, public with_pimpl<XLSBParser>
{
	private:
		friend pimpl_impl<XLSBParser>;
		using with_pimpl<XLSBParser>::impl;
		using with_pimpl<XLSBParser>::renew_impl;
		attributes::Metadata metaData(ZipReader& unzip);

	public:
		XLSBParser();
		XLSBParser(XLSBParser&&);
		~XLSBParser();
		inline static const std::vector<mime_type> supported_mime_types =
		{
			mime_type{"application/vnd.ms-excel.sheet.binary.macroenabled.12"}
		};
		void parse(const data_source& data) override;
};

} // namespace docwire

#endif
