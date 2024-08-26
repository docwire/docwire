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

#ifndef DOCWIRE_ODFOOXML_PARSER_H
#define DOCWIRE_ODFOOXML_PARSER_H

#include "common_xml_document_parser.h"
#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class DllExport ODFOOXMLParser : public Parser,
                       public CommonXMLDocumentParser
{
  private:
    struct ExtendedImplementation;
    std::unique_ptr<ExtendedImplementation> extended_impl;
    class CommandHandlersSet;
    int lastOOXMLRowNum();
    void setLastOOXMLRowNum(int r);
    int lastOOXMLColNum();
    void setLastOOXMLColNum(int c);
  void onOOXMLBreak(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                     const ZipReader* zipfile, std::string& text,
                     bool& children_processed, std::string& level_suffix, bool first_on_level) const;
    void parse(const data_source& data, XmlParseMode mode) const;
    attributes::Metadata metaData(ZipReader& zipfile) const;

	public:

    void parse(const data_source& data) const override;
    Parser& addOnNewNodeCallback(NewNodeCallback callback) override;
    static std::vector<file_extension> getExtensions()
    {
      return { file_extension{".odt"}, file_extension{".ods"}, file_extension{".odp"}, file_extension{".odg"}, file_extension{".docx"}, file_extension{".xlsx"}, file_extension{".pptx"}, file_extension{".ppsx"}};
    }
    Parser& withParameters(const ParserParameters &parameters) override;

    ODFOOXMLParser();
    ~ODFOOXMLParser();
    bool understands(const data_source& data) const override;
};

} // namespace docwire

#endif
