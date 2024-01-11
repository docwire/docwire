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

#ifndef DOCWIRE_XLS_PARSER_H
#define DOCWIRE_XLS_PARSER_H

#include "metadata.h"
#include <string>
#include <vector>

namespace docwire
{

class ThreadSafeOLEStorage;

	struct FormattingStyle;

class XLSParser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		XLSParser(const std::string& file_name);
		XLSParser(const char* buffer, size_t size);
		~XLSParser();
    static std::vector<std::string> getExtensions() {return {"xls"};}
		bool isXLS();
		std::string plainText(const FormattingStyle& formatting);
		std::string plainText(ThreadSafeOLEStorage& storage, const FormattingStyle& formatting);
		Metadata metaData();
};

} // namespace docwire

#endif
