/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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
