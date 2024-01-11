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

#ifndef DOCWIRE_TXT_PARSER_H
#define DOCWIRE_TXT_PARSER_H

#include <string>

#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class TXTParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:

    void parse() const override;
    Parser& addOnNewNodeCallback(NewNodeCallback callback) override;
		Parser& withParameters(const ParserParameters &parameters) override;
		static std::vector <std::string> getExtensions();

		TXTParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		TXTParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		~TXTParser();
		std::string plainText() const;
};

} // namespace docwire

#endif
