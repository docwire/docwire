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

#include <algorithm>
#include <string>
#include <vector>

#include "office_formats_parser_provider.h"
#include "parser_wrapper.h"
#include "html_parser.h"
#include "doc_parser.h"
#include "pdf_parser.h"
#include "odfxml_parser.h"
#include "xls_parser.h"
#include "xlsb_parser.h"
#include "iwork_parser.h"
#include "ppt_parser.h"
#include "rtf_parser.h"
#include "odf_ooxml_parser.h"
#include "txt_parser.h"

namespace docwire
{

void
OfficeFormatsParserProvider::addExtensions(const std::vector<file_extension> &extensions)
{
  available_extensions.insert(extensions.begin(), extensions.end());
}

OfficeFormatsParserProvider::OfficeFormatsParserProvider()
{
  addExtensions(HTMLParser::getExtensions());
  addExtensions(DOCParser::getExtensions());
  addExtensions(PDFParser::getExtensions());
  addExtensions(XLSParser::getExtensions());
  addExtensions(XLSBParser::getExtensions());
  addExtensions(IWorkParser::getExtensions());
  addExtensions(PPTParser::getExtensions());
  addExtensions(RTFParser::getExtensions());
  addExtensions(ODFXMLParser::getExtensions());
  addExtensions(ODFOOXMLParser::getExtensions());
  addExtensions(TXTParser::getExtensions());
}

bool
OfficeFormatsParserProvider::isExtensionInVector(const file_extension& extension, const std::vector<file_extension>& extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

std::unique_ptr<ParserBuilder>
OfficeFormatsParserProvider::findParserByExtension(const file_extension& inExtension) const
{
  if (isExtensionInVector(inExtension, HTMLParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<HTMLParser>>();
  }
  else if (isExtensionInVector(inExtension, DOCParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<DOCParser>>();
  }
  else if (isExtensionInVector(inExtension, PDFParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<PDFParser>>();
  }
  else if (isExtensionInVector(inExtension, XLSParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<XLSParser>>();
  }
  else if (isExtensionInVector(inExtension, XLSBParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<XLSBParser>>();
  }
  else if (isExtensionInVector(inExtension, IWorkParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<IWorkParser>>();
  }
  else if (isExtensionInVector(inExtension, PPTParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<PPTParser>>();
  }
  else if (isExtensionInVector(inExtension, RTFParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<RTFParser>>();
  }
  else if (isExtensionInVector(inExtension, ODFXMLParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<ODFXMLParser>>();
  }
  else if (isExtensionInVector(inExtension, ODFOOXMLParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<ODFOOXMLParser>>();
  }
  else if (isExtensionInVector(inExtension, TXTParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<TXTParser>>();
  }
  return nullptr;
}

template <typename T, bool(T::*valid_method)() const>
bool
is_valid(const char* buffer, size_t size)
{
  T parser(buffer, size);
  return (parser.*valid_method)();
}

template <typename T>
bool parser_understands(const data_source& data)
{
  docwire_log_func();
  T parser;
  return parser.understands(data);
}

std::unique_ptr<ParserBuilder>
OfficeFormatsParserProvider::findParserByData(const data_source& data) const
{
  docwire_log_func();
  if (parser_understands<HTMLParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<HTMLParser>>();
  }
  else if (parser_understands<DOCParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<DOCParser>>();
  }
  else if (parser_understands<PDFParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<PDFParser>>();
  }
  else if (parser_understands<XLSParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<XLSParser>>();
  }
  else if (parser_understands<XLSBParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<XLSBParser>>();
  }
  else if (parser_understands<IWorkParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<IWorkParser>>();
  }
  else if (parser_understands<PPTParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<PPTParser>>();
  }
  else if (parser_understands<RTFParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<RTFParser>>();
  }
  else if (parser_understands<ODFOOXMLParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<ODFOOXMLParser>>();
  }
  else if (parser_understands<ODFXMLParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<ODFXMLParser>>();
  }
  return nullptr;
}

std::set<file_extension> OfficeFormatsParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

} // namespace docwire
