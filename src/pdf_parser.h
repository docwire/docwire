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
#include <vector>

namespace docwire
{

struct FormattingStyle;
class Metadata;

class PDFParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		PDFParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		PDFParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		~PDFParser();
    void parse() const override;
    static std::vector<std::string> getExtensions() {return {"pdf"};}

		bool isPDF();
		Metadata metaData();
};

} // namespace docwire

#endif
