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

#ifndef DOCWIRE_OCR_PARSER_PROVIDER_H
#define DOCWIRE_OCR_PARSER_PROVIDER_H

#include "parser_provider.h"
#include "defines.h"

namespace docwire
{

class DllExport OcrParserProvider : public ParserProvider
{
public:
  OcrParserProvider();
  std::optional<ParserBuilder*> findParserByExtension(const std::string &inExtension) const override;
  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const override;
  std::set<std::string> getAvailableExtensions() const override;

private:
  void addExtensions(const std::vector<std::string> &inExtensions);
  bool isExtensionInVector(const std::string &extension, const std::vector<std::string> &extension_list) const;
  std::set<std::string> available_extensions;
};

extern "C" DllExport OcrParserProvider plugin_parser_provider;
OcrParserProvider plugin_parser_provider;

} // namespace docwire

#endif //DOCWIRE_OCR_PARSER_PROVIDER_H
