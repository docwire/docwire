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
#include "parser_builder.h"

namespace docwire
{

class DllExport ODFXMLParser : public CommonXMLDocumentParser
{
	private:
		struct ExtendedImplementation;
		std::unique_ptr<ExtendedImplementation> extended_impl;
		class CommandHandlersSet;
		attributes::Metadata metaData(const std::string& xml_content) const;
		void parse(const data_source& data, XmlParseMode mode) const;

	public:

    void parse(const data_source& data) const override;

    	static std::vector<file_extension> getExtensions()
		{
			return { file_extension{".fodt"}, file_extension{".fods"}, file_extension{".fodp"}, file_extension{".fodg"} };
		}
		Parser& withParameters(const ParserParameters &parameters) override;

		ODFXMLParser();
		~ODFXMLParser();
		bool understands(const data_source& data) const override;
};

} // namespace docwire

#endif
