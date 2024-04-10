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
OfficeFormatsParserProvider::addExtensions(const std::vector<std::string> &extensions)
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
OfficeFormatsParserProvider::isExtensionInVector(const std::string &extension, const std::vector<std::string> &extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

std::unique_ptr<ParserBuilder>
OfficeFormatsParserProvider::findParserByExtension(const std::string &inExtension) const
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

template <typename T, bool(T::*valid_method)()>
bool
is_valid(const char* buffer, size_t size)
{
  T parser(buffer, size);
  return (parser.*valid_method)();
}

std::unique_ptr<ParserBuilder>
OfficeFormatsParserProvider::findParserByData(const std::vector<char>& buffer) const
{
  if (is_valid<HTMLParser, &HTMLParser::isHTML>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<HTMLParser>>();
  }
  else if (is_valid<DOCParser, &DOCParser::isDOC>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<DOCParser>>();
  }
  else if (is_valid<PDFParser, &PDFParser::isPDF>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<PDFParser>>();
  }
  else if (is_valid<XLSParser, &XLSParser::isXLS>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<XLSParser>>();
  }
  else if (is_valid<XLSBParser, &XLSBParser::isXLSB>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<XLSBParser>>();
  }
  else if (is_valid<IWorkParser, &IWorkParser::isIWork>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<IWorkParser>>();
  }
  else if (is_valid<PPTParser, &PPTParser::isPPT>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<PPTParser>>();
  }
  else if (is_valid<RTFParser, &RTFParser::isRTF>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<RTFParser>>();
  }
  else if (is_valid<ODFOOXMLParser, &ODFOOXMLParser::isODFOOXML>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<ODFOOXMLParser>>();
  }
  else if (is_valid<ODFXMLParser, &ODFXMLParser::isODFXML>(buffer.data(), buffer.size()))
  {
    return std::make_unique<ParserBuilderWrapper<ODFXMLParser>>();
  }
  return nullptr;
}

std::set<std::string>
OfficeFormatsParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

} // namespace docwire
