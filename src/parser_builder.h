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


#ifndef DOCWIRE_PARSER_BUILDER_H
#define DOCWIRE_PARSER_BUILDER_H

#include <vector>
#include <memory>
#include <functional>
#include "parser.h"
#include "parser_manager.h"
#include "parser_parameters.h"
#include "defines.h"

namespace docwire
{
/**
 * Abstract class to build parsers. Parser could be built from path to file or from data buffer.
 */
class DllExport ParserBuilder
{
public:
  /**
   * @brief Builds new parser object.
   * @param inFileName path to file
   * @return pointer to new parser object
   */
  virtual std::unique_ptr<Parser> build(const std::string &inFileName) const = 0;

  /**
   *
   * @brief Builds new parser object.
   * @param buffer raw data of file to be parsed
   * @param size file size
   * @return pointer to new parser object
   */
  virtual std::unique_ptr<Parser> build(const char *buffer, size_t size) const = 0;

  /**
   * @brief Adds callback function.
   * @param callbacks
   */
  virtual ParserBuilder &withOnNewNodeCallbacks(const std::vector<NewNodeCallback> &callbacks) = 0;

  /**
   * @brief Sets parser manager.
   * @param inParserManager
   */
  virtual ParserBuilder &withParserManager(const std::shared_ptr<ParserManager> &inParserManager) = 0;

  /**
   * @brief Sets parser parameters.
   * @param inParameters
   */
  virtual ParserBuilder &withParameters(const ParserParameters &inParameters) = 0;

  virtual ~ParserBuilder() = default;
};
} // namespace docwire
#endif //DOCWIRE_PARSER_BUILDER_H
