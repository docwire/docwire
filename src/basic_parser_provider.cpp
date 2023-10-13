/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include <algorithm>
#include <string>
#include <vector>

#include "basic_parser_provider.h"
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
BasicParserProvider::addExtensions(const std::vector<std::string> &extensions)
{
  available_extensions.insert(extensions.begin(), extensions.end());
}

BasicParserProvider::BasicParserProvider()
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
BasicParserProvider::isExtensionInVector(const std::string &extension, const std::vector<std::string> &extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

std::optional<ParserBuilder*>
BasicParserProvider::findParserByExtension(const std::string &inExtension) const
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

std::optional<ParserBuilder*>
BasicParserProvider::findParserByData(const std::vector<char>& buffer) const
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
BasicParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

} // namespace docwire
