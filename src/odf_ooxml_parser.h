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

namespace docwire
{

class DllExport ODFOOXMLParser : public CommonXMLDocumentParser, public with_pimpl<ODFOOXMLParser>
{
  private:
    using with_pimpl<ODFOOXMLParser>::impl;
    class CommandHandlersSet;
    int lastOOXMLRowNum();
    void setLastOOXMLRowNum(int r);
    int lastOOXMLColNum();
    void setLastOOXMLColNum(int c);
  void onOOXMLBreak(CommonXMLDocumentParser& parser, XmlStream& xml_stream, XmlParseMode mode,
                     const ZipReader* zipfile, std::string& text,
                     bool& children_processed, std::string& level_suffix, bool first_on_level) const;
    void parse(const data_source& data, XmlParseMode mode);
    attributes::Metadata metaData(ZipReader& zipfile) const;

	public:

    void parse(const data_source& data) override;

    inline static const std::vector<mime_type> supported_mime_types =
    {
      mime_type{"application/vnd.oasis.opendocument.text"},
      mime_type{"application/vnd.oasis.opendocument.spreadsheet"},
      mime_type{"application/vnd.oasis.opendocument.presentation"},
      mime_type{"application/vnd.oasis.opendocument.graphics"},
      mime_type{"application/vnd.oasis.opendocument.text-template"},
      mime_type{"application/vnd.oasis.opendocument.spreadsheet-template"},
      mime_type{"application/vnd.oasis.opendocument.presentation-template"},
      mime_type{"application/vnd.oasis.opendocument.graphics-template"},
      mime_type{"application/vnd.oasis.opendocument.text-web"},
      mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
      mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
      mime_type{"application/vnd.openxmlformats-officedocument.presentationml.presentation"},
      mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
      mime_type{"application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
      mime_type{"application/vnd.openxmlformats-officedocument.presentationml.template"},
      mime_type{"application/vnd.openxmlformats-officedocument.presentationml.slideshow"},
    };

    ODFOOXMLParser();
};

} // namespace docwire

#endif
