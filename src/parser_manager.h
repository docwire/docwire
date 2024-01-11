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

#ifndef DOCWIRE_PARSER_MANAGER_H
#define DOCWIRE_PARSER_MANAGER_H

#include <optional>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include "defines.h"

namespace docwire
{

class ParserBuilder;
class ParserProvider;

/**
 * @brief Parser manager class. Loads all available parsers and provides access to them.
 */
class DllExport ParserManager
{
public:
    ParserManager();
    /**
     * @param plugins_directory localization with plugins to be loaded
     */
    explicit ParserManager(const std::string &plugins_directory);

    ~ParserManager();
  /**
   * @brief Returns parser builder for given extension type or nullopt if no parser is found.
   * @param file_name file name with extension (e.g. ".txt", ".docx", etc.)
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByExtension(const std::string &file_name) const;

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param buffer buffer of raw data
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const;

  /**
   * @brief Returns all available parsers.
   * @return sets of all available parsers
   */
  std::set<std::string> getAvailableExtensions() const;


private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

} // namespace docwire
#endif //DOCWIRE_PARSER_MANAGER_H
