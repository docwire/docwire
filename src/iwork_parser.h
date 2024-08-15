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

#ifndef DOCWIRE_IWORK_PARSER_H
#define DOCWIRE_IWORK_PARSER_H

#include "exception.h"
#include "parser.h"
#include <string>
#include "tags.h"
#include <vector>

namespace docwire
{

class Metadata;

class DllExport IWorkParser : public Parser
{
	private:
		struct Implementation;
		std::unique_ptr<Implementation> impl;
		attributes::Metadata metaData(std::shared_ptr<std::istream> stream) const;

	public:
		IWorkParser();
		~IWorkParser();
    	static std::vector<file_extension> getExtensions()
		{
			return { file_extension{".pages"}, file_extension{".key"}, file_extension{".numbers"} };
		}
		bool understands(const data_source& data) const override;

		void parse(const data_source& data) const override;

	DOCWIRE_EXCEPTION_DEFINE(UnzipError, RuntimeError);
	DOCWIRE_EXCEPTION_DEFINE(ParsingError, RuntimeError);
};

} // namespace docwire

#endif
