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

std::unique_ptr<ParserBuilder>
MailParserProvider::findParserByExtension(const file_extension& inExtension) const
{
  if (isExtensionInVector(inExtension, EMLParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<EMLParser>>();
  }
  else if(isExtensionInVector(inExtension, PSTParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<PSTParser>>();
  }
  return nullptr;
}

template <typename T>
bool parser_understands(const data_source& data)
{
  docwire_log_func();
  T parser;
  return parser.understands(data);
}

std::unique_ptr<ParserBuilder>
MailParserProvider::findParserByData(const data_source& data) const
{
  docwire_log_func();
  if (parser_understands<EMLParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<EMLParser>>();
  }
  else if (parser_understands<PSTParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<PSTParser>>();
  }
  return nullptr;
}

std::set<file_extension> MailParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

void MailParserProvider::addExtensions(const std::vector<file_extension> &inExtensions)
{
  available_extensions.insert(inExtensions.begin(), inExtensions.end());
}

bool MailParserProvider::isExtensionInVector(const file_extension& extension, const std::vector<file_extension>& extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

} // namespace docwire
