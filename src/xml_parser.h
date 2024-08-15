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

#ifndef DOCWIRE_XML_PARSER_H
#define DOCWIRE_XML_PARSER_H

#include "parser.h"

namespace docwire
{

class DllExport XMLParser : public Parser
{
public:
	void parse(const data_source& data) const override;
    static std::vector<file_extension> getExtensions() { return { file_extension{".xml"} }; }
	bool understands(const data_source& data) const;
};

} // namespace docwire

#endif // DOCWIRE_XML_PARSER_H
