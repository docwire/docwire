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


#ifndef DOCWIRE_PARSER_PROVIDER_H
#define DOCWIRE_PARSER_PROVIDER_H

#include <optional>

#include <set>
#include "parser_builder.h"
#include "defines.h"

namespace docwire
{

/**
 * @brief The ParserProvider class.
 */
class DllExport ParserProvider
{
public:
  /**
   * @brief Returns parser builder for given extension type or nullopt if no parser is found.
   * @param inExtension file extension (e.g. ".txt", ".docx", etc.)
   * @return specific parser builder or nullopt if no parser is found
   */
  virtual std::optional<ParserBuilder*> findParserByExtension(const std::string &extension) const = 0;

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param buffer buffer of raw data
   * @return specific parser builder or nullopt if no parser is found
   */
  virtual std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const = 0;

  /**
   * @brief Returns all available parsers.
   * @return sets of all available parsers
   */
  virtual std::set<std::string> getAvailableExtensions() const = 0;
};

} // namespace docwire

#endif //DOCWIRE_PARSER_PROVIDER_H
