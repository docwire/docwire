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

#include "mail_parser_provider.h"
#include "parser_wrapper.h"

#include "eml_parser.h"
#include "pst_parser.h"

namespace docwire
{

MailParserProvider::MailParserProvider()
{
  addExtensions(EMLParser::getExtensions());
  addExtensions(PSTParser::getExtensions());
}

std::optional<ParserBuilder*>
MailParserProvider::findParserByExtension(const std::string &inExtension) const
{
  if (isExtensionInVector(inExtension, EMLParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<EMLParser>>();
  }
  else if(isExtensionInVector(inExtension, PSTParser::getExtensions()))
  {
    return new ParserBuilderWrapper<parser_creator<PSTParser>>();
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
MailParserProvider::findParserByData(const std::vector<char>& buffer) const
{
  if (is_valid<EMLParser, &EMLParser::isEML>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<wrapper_parser_creator<EMLParser>>();
  }
  else if (is_valid<PSTParser, &PSTParser::isPST>(buffer.data(), buffer.size()))
  {
    return new ParserBuilderWrapper<parser_creator<PSTParser>>();
  }
  return std::nullopt;
}

std::set<std::string>
MailParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

void
MailParserProvider::addExtensions(const std::vector<std::string> &inExtensions)
{
  available_extensions.insert(inExtensions.begin(), inExtensions.end());
}

bool
MailParserProvider::isExtensionInVector(const std::string &extension, const std::vector<std::string> &extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

} // namespace docwire
