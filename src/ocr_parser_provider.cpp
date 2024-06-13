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

#include "ocr_parser_provider.h"
#include "parser_wrapper.h"

#include "ocr_parser.h"

namespace docwire
{

OcrParserProvider::OcrParserProvider()
{
  addExtensions(OCRParser::getExtensions());
}

std::unique_ptr<ParserBuilder>
OcrParserProvider::findParserByExtension(const file_extension& inExtension) const
{
  if (isExtensionInVector(inExtension, OCRParser::getExtensions()))
  {
    return std::make_unique<ParserBuilderWrapper<OCRParser>>();
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
OcrParserProvider::findParserByData(const data_source& data) const
{
  docwire_log_func();
  if (parser_understands<OCRParser>(data))
  {
    return std::make_unique<ParserBuilderWrapper<OCRParser>>();
  }
  return nullptr;
}

std::set<file_extension> OcrParserProvider::getAvailableExtensions() const
{
  return available_extensions;
}

void OcrParserProvider::addExtensions(const std::vector<file_extension> &inExtensions)
{
  available_extensions.insert(inExtensions.begin(), inExtensions.end());
}

bool OcrParserProvider::isExtensionInVector(const file_extension& extension, const std::vector<file_extension>& extension_list) const
{
  return std::find(extension_list.begin(), extension_list.end(), extension) != extension_list.end();
}

} // namespace docwire
