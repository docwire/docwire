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

class DllExport TXTParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:

    void parse(const data_source& data) const override;
	bool understands(const data_source& data) const override;
		Parser& withParameters(const ParserParameters &parameters) override;
		static std::vector<file_extension> getExtensions();

		TXTParser();
	static constexpr bool is_universal = true;
};

} // namespace docwire

#endif
