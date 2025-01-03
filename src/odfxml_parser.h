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

namespace docwire
{

class DllExport ODFXMLParser : public CommonXMLDocumentParser
{
	private:
		class CommandHandlersSet;
		attributes::Metadata metaData(const std::string& xml_content) const;
		void parse(const data_source& data, XmlParseMode mode);

	public:

    void parse(const data_source& data) override;

		inline static const std::vector<mime_type> supported_mime_types =
		{
			mime_type{"application/vnd.oasis.opendocument.text-flat-xml"},
			mime_type{"application/vnd.oasis.opendocument.spreadsheet-flat-xml"},
			mime_type{"application/vnd.oasis.opendocument.presentation-flat-xml"},
			mime_type{"application/vnd.oasis.opendocument.graphics-flat-xml"}
		};

		Parser& withParameters(const ParserParameters &parameters) override;

		ODFXMLParser();
};

} // namespace docwire

#endif
