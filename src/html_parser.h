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

#ifndef DOCWIRE_HTML_PARSER_H
#define DOCWIRE_HTML_PARSER_H

#include <string>
#include <vector>
#include "parser.h"
#include "parser_builder.h"
#include "parser_manager.h"

namespace docwire
{
	class Metadata;

class HTMLParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;
		friend class SaxParser;

	public:

    void parse() const override;
    static std::vector <std::string> getExtensions() {return {"html", "htm"};}
		Parser& withParameters(const ParserParameters &parameters) override;

		explicit HTMLParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		HTMLParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		~HTMLParser();
		bool isHTML();
		Metadata metaData() const;
		///turns off charset decoding. It may be useful, if we want to decode data ourself (EML parser is an example).
		void skipCharsetDecoding();
};

} // namespace docwire

#endif
