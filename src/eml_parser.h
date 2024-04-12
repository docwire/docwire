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

#ifndef DOCWIRE_EML_PARSER_H
#define DOCWIRE_EML_PARSER_H

#include <string>
#include <vector>

#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class Metadata;
class Attachment;

class EMLParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;
		tag::Metadata metaData();

	public:
		EMLParser(const std::string& file_name);
		EMLParser(const char* buffer, size_t size);
		~EMLParser();
		void parse() const override;
		static std::vector<std::string> getExtensions() {return {"eml"};}

		bool isEML() const;
};

} // namespace docwire

#endif
