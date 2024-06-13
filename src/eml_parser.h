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
		std::unique_ptr<Implementation> impl;

	public:
		EMLParser();
		~EMLParser();
		void parse(const data_source& data) const override;
		static std::vector<file_extension> getExtensions() { return { file_extension{".eml"}}; }

		bool understands(const data_source& data) const override;
};

} // namespace docwire

#endif
