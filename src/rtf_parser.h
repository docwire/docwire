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

#ifndef DOCWIRE_RTF_PARSER_H
#define DOCWIRE_RTF_PARSER_H

#include "parser.h"
#include "parser_builder.h"
#include <string>
#include <vector>

namespace docwire
{
	class Metadata;

class RTFParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:

    void parse() const override;
		Parser& addOnNewNodeCallback(NewNodeCallback callback) override;
		Parser& withParameters(const ParserParameters &parameters) override;
    static std::vector <std::string> getExtensions() {return {"rtf"};}

		RTFParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		RTFParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		~RTFParser();
		bool isRTF() const;
		std::string plainText() const;
		Metadata metaData() const;
};

} // namespace docwire

#endif
