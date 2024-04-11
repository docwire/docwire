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

#ifndef DOCWIRE_ODFXML_PARSER_H
#define DOCWIRE_ODFXML_PARSER_H

#include "common_xml_document_parser.h"
#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class ODFXMLParser : public Parser,
                     public CommonXMLDocumentParser
{
	private:
		struct ExtendedImplementation;
		ExtendedImplementation* extended_impl;
		class CommandHandlersSet;

	public:

    void parse() const override;
    Parser& addOnNewNodeCallback(NewNodeCallback callback) override;
    static std::vector <std::string> getExtensions() {return {"fodt", "fods", "fodp", "fodg"};}
		Parser& withParameters(const ParserParameters &parameters) override;

		ODFXMLParser(const std::string &file_name);
		ODFXMLParser(const char* buffer, size_t size);
		~ODFXMLParser();
		bool isODFXML();
		void plainText(XmlParseMode mode) const;
		tag::Metadata metaData() const;
};

} // namespace docwire

#endif
