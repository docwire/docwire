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

#ifndef DOCWIRE_PPT_PARSER_H
#define DOCWIRE_PPT_PARSER_H

#include "parser.h"
#include <string>
#include "tags.h"
#include <vector>

namespace docwire
{

struct FormattingStyle;
struct Metadata;

class PPTParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		PPTParser(const std::string& file_name);
		PPTParser(const char* buffer, size_t size);
		~PPTParser();
    static std::vector<std::string> getExtensions() {return {"ppt", "pps"};}
		bool isPPT();
		std::string plainText(const FormattingStyle& formatting) const;
		tag::Metadata metaData() const;

		void parse() const override
		{
			sendTag(tag::Text{.text = plainText(getFormattingStyle())});
			sendTag(metaData());
		}
};

} // namespace docwire

#endif
