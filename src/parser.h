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


#ifndef DOCWIRE_PARSER_H
#define DOCWIRE_PARSER_H

#include <any>
#include <string>
#include <functional>
#include <memory>

#include "parser_parameters.h"
#include "tags.h"
#include "defines.h"

namespace docwire
{

struct DllExport Info
{
  Tag tag;
  bool cancel = false; //!< cancel flag. If set true then parsing process will be stopped.
  bool skip = false; //!< skip flag. If set true then tag will be skipped.

  explicit Info(const Tag& tag)
    : tag(tag)
  {}
};

typedef std::function<void(Info &info)> NewNodeCallback;

/**
 * @brief Abstract class for all parsers
 */
class DllExport Parser
{
public:
  explicit Parser();
  Parser(Parser &&) = default;
  virtual ~Parser() = default;

  enum class parsing_continuation { proceed, skip, stop };

  /**
   * @brief Start parsing process.
   * @param data data to parse
   * @param callback function to execute for every document node. Nodes depends on the kind of parser.
   * It can be email for pst file, page or paragraph for pdf file etc.
   */
  void operator()(const data_source& data, std::function<parsing_continuation(const Tag&)> callback) const;

  /**
   * @brief Checks if parser can parse specified data
  */
  virtual bool understands(const data_source& data) const = 0;

  virtual Parser &withParameters(const ParserParameters &parameters);

protected:
  /**
   * @brief Executes text parsing
   */
  virtual void parse(const data_source& data) const = 0;

  Info sendTag(const Tag& tag) const;
  Info sendTag(const Info &info) const;

  ParserParameters m_parameters;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> base_impl;
};

} // namespace docwire
#endif //DOCWIRE_PARSER_H
