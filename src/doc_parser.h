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

#ifndef DOCWIRE_DOC_PARSER_H
#define DOCWIRE_DOC_PARSER_H

#include "exception.h"
#include "parser.h"
#include "parser_builder.h"
#include <string>
#include <vector>

namespace docwire
{
	class Metadata;

class DOCParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;
		friend class TextHandler;
		friend class SubDocumentHandler;
		friend class TableHandler;
		tag::Metadata metaData() const;

	public:

    void parse() const override;
		Parser& withParameters(const ParserParameters &parameters) override;
    static std::vector <std::string> getExtensions() {return {"doc", "dot"};}

		DOCParser(const std::string& file_name);
		DOCParser(const char* buffer, size_t size);
		~DOCParser();
		bool isDOC();

	DOCWIRE_EXCEPTION_DEFINE(ParsingError, RuntimeError);
};

} // namespace docwire

#endif
