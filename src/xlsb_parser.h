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

class DllExport XLSBParser : public Parser
{
	private:
		struct Implementation;
		std::unique_ptr<Implementation> impl;
		attributes::Metadata metaData(const ZipReader& unzip) const;

	public:
		XLSBParser();
		~XLSBParser();
    	static std::vector<file_extension> getExtensions() {return { file_extension{".xlsb"} };}
		bool understands(const data_source& data) const override;
		void parse(const data_source& data) const override;
};

} // namespace docwire

#endif
