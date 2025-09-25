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
#include "data_source.h"
#include "odf_ooxml_export.h"

namespace docwire
{

class DOCWIRE_ODF_OOXML_EXPORT ODFOOXMLParser : public CommonXMLDocumentParser, public with_pimpl<ODFOOXMLParser>
{
  private:
    using with_pimpl<ODFOOXMLParser>::impl;
    friend pimpl_impl<ODFOOXMLParser>;
    class CommandHandlersSet;
    int lastOOXMLRowNum();
    void setLastOOXMLRowNum(int r);
    int lastOOXMLColNum();
    void setLastOOXMLColNum(int c);
    void parse(const data_source& data, XmlParseMode mode, const message_callbacks& emit_message);
    attributes::Metadata metaData(ZipReader& zipfile) const;

	public:

    void parse(const data_source& data, const message_callbacks& emit_message);


    ODFOOXMLParser();
    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
    bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
