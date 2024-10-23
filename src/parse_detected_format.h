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

#ifndef DOCWIRE_PARSE_DETECTED_FORMAT_H
#define DOCWIRE_PARSE_DETECTED_FORMAT_H

#include <algorithm>
#include <memory>

#include "importer.h"
#include "parser_builder.h"
#include "parser_parameters.h"

namespace docwire
{

class ParserProvider;

template<typename... ProviderTypeNames>
std::unique_ptr<ParserBuilder> find_parser_by_mime_type(const mime_type& mime)
{
  std::unique_ptr<ParserProvider> providers[] = {std::unique_ptr<ParserProvider>(new ProviderTypeNames)...};
  for (auto& provider : providers)
  {
    auto builder = provider->find_parser_by_mime_type(mime);
    if (builder)
      return builder;
  }
  return nullptr;
}

template<typename... ProviderTypeNames>
std::unique_ptr<ParserBuilder> findParserByData(const data_source& data)
{
  std::unique_ptr<ParserProvider> providers[] = {std::unique_ptr<ParserProvider>(new ProviderTypeNames)...};
  for (auto& provider : providers)
  {
    auto builder = provider->findParserByData(data);
    if (builder)
      return builder;
  }
  return nullptr;
}

/**
 * @brief This class template is used to import a file and parse it using available parsers.
 * @code
 * Input("file.pdf") | ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>(parameters) | HtmlExporter() | std::cout; // Imports file.pdf and exports it to std::cout as HTML
 * @endcode
 *
 * @see Parser
 */
template<typename ProviderTypeName, typename... ProviderTypeNames>
class ParseDetectedFormat : public Importer
{
  public:
    ParseDetectedFormat(const ParserParameters &parameters = ParserParameters())
      : Importer(parameters)
    {}

  /**
   * @brief Returns parser builder for given mime type or nullopt if no parser is found.
   * @param mime mime type (e.g. "text/plain", "application/pdf", etc.)
   * @return specific parser builder or null unique_ptr if no parser is found
   */
  std::unique_ptr<ParserBuilder> find_parser_by_mime_type(const mime_type& mime) const override
  {
    return docwire::find_parser_by_mime_type<ProviderTypeName, ProviderTypeNames...>(mime);
  }

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param buffer buffer of raw data
   * @return specific parser builder or null unique_ptr if no parser is found
   */
  std::unique_ptr<ParserBuilder> findParserByData(const data_source& data) const override
  {
    return docwire::findParserByData<ProviderTypeName, ProviderTypeNames...>(data);
  }
};

} // namespace docwire

#endif //DOCWIRE_PARSE_DETECTED_FORMAT_H
