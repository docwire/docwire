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

#include "formatting_style.h"
#include "parser_parameters.h"
#include "tags.h"
#include "defines.h"

namespace docwire
{
class Importer;

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
  /**
   *
   * @param inImporter importer known about all available parsers which could be used recursive
   */
  explicit Parser(const Importer* inImporter = nullptr);

  virtual ~Parser() = default;

  /**
   * @brief Executes text parsing
   */
  virtual void parse() const = 0;
  /**
   * @brief Adds new function to execute when new node will be created. Node is a part of parsed text.
   * Depends on the kind of parser it could be. For example, email from pst file or page from pdf file.
   * @param callback function to execute
   * @return reference to self
   */
  virtual Parser &addOnNewNodeCallback(NewNodeCallback callback);

  virtual Parser &withParameters(const ParserParameters &parameters);

protected:
  /**
  * @brief Loads FormattingStyle from ParserParameters.
  * @return Loaded FormattingStyle if exists, otherwise defualt FormattingStyle .
  **/
  FormattingStyle getFormattingStyle() const;

  Info sendTag(const Tag& tag) const;
  Info sendTag(const Info &info) const;

  const Importer* m_importer;
  ParserParameters m_parameters;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> base_impl;
};

} // namespace docwire
#endif //DOCWIRE_PARSER_H
