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

#ifndef DOCWIRE_EML_PARSER_H
#define DOCWIRE_EML_PARSER_H

#include <string>
#include <vector>

#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

struct FormattingStyle;
class Metadata;
class Attachment;

class EMLParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		EMLParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		EMLParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
		~EMLParser();
		void parse() const override;
		static std::vector<std::string> getExtensions() {return {"eml"};}

		bool isEML() const;
		std::string plainText(const FormattingStyle& formatting) const;
		Metadata metaData();
};

} // namespace docwire

#endif
