/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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

#include <string>
#include <vector>

namespace docwire
{
	struct FormattingStyle;
	class Metadata;
}

using namespace docwire;

class XLSBParser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		XLSBParser(const std::string& file_name);
		XLSBParser(const char* buffer, size_t size);
		~XLSBParser();
    static std::vector<std::string> getExtensions() {return {"xlsb"};}
		bool isXLSB();
		std::string plainText(const FormattingStyle& formatting);
		Metadata metaData();
};

#endif
