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

std::unique_ptr<ParserBuilder*>
OfficeFormatsParserProvider::findParserByExtension(const std::string &inExtension) const
{
  if (isExtensionInVector(inExtension, HTMLParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<HTMLParser>>();
  }
  else if (isExtensionInVector(inExtension, DOCParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<DOCParser>>();
  }
  else if (isExtensionInVector(inExtension, PDFParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<PDFParser>>();
  }
  else if (isExtensionInVector(inExtension, XLSParser::getExtensions()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<XLSParser>>();
  }
  else if (isExtensionInVector(inExtension, XLSBParser::getExtensions()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<XLSBParser>>();
  }
  else if (isExtensionInVector(inExtension, IWorkParser::getExtensions()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<IWorkParser>>();
  }
  else if (isExtensionInVector(inExtension, PPTParser::getExtensions()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<PPTParser>>();
  }
  else if (isExtensionInVector(inExtension, RTFParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<RTFParser>>();
  }
  else if (isExtensionInVector(inExtension, ODFXMLParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<ODFXMLParser>>();
  }
  else if (isExtensionInVector(inExtension, ODFOOXMLParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<ODFOOXMLParser>>();
  }
  else if (isExtensionInVector(inExtension, TXTParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<TXTParser>>();
  }
  return std::nullopt;
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
    return new ParserBuilderWrapper<parser_creator<HTMLParser>>();
  }
  else if (is_valid<DOCParser, &DOCParser::isDOC>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<DOCParser>>();
  }
  else if (is_valid<PDFParser, &PDFParser::isPDF>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<PDFParser>>();
  }
  else if (is_valid<XLSParser, &XLSParser::isXLS>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<XLSParser>>();
  }
  else if (is_valid<XLSBParser, &XLSBParser::isXLSB>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<XLSBParser>>();
  }
  else if (is_valid<IWorkParser, &IWorkParser::isIWork>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<IWorkParser>>();
  }
  else if (is_valid<PPTParser, &PPTParser::isPPT>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<PPTParser>>();
  }
  else if (is_valid<RTFParser, &RTFParser::isRTF>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<RTFParser>>();
  }
  else if (is_valid<ODFOOXMLParser, &ODFOOXMLParser::isODFOOXML>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<ODFOOXMLParser>>();
  }
  else if (is_valid<ODFXMLParser, &ODFXMLParser::isODFXML>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<ODFXMLParser>>();
  }
  return std::nullopt;
}

std::set<std::string>
OfficeFormatsParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

} // namespace docwire
