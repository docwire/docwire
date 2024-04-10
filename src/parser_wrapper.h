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

#ifndef DOCWIRE_PARSER_WRAPPER_H
#define DOCWIRE_PARSER_WRAPPER_H

#include <iostream>

#include "formatting_style.h"
#include "parser.h"
#include "parser_builder.h"
#include "defines.h"

namespace docwire
{

class Importer;

/**
 * @brief Provides the basic mechanism to build any parser.
 * @tparam ParserType type of parser to build
 */
template<typename ParserType>
class DllExport ParserBuilderWrapper : public ParserBuilder
{
public:
  ParserBuilderWrapper()
  {

  }

  std::unique_ptr<Parser>
  build(const std::string &inFileName) const override
  {
    auto parser = std::make_unique<ParserType>(inFileName, m_importer);
    for (auto &callback : m_callbacks)
    {
      parser->addOnNewNodeCallback(callback);
    }
    parser->withParameters(m_parameters);
    return parser;
  }

  std::unique_ptr<Parser>
  build(const char* buffer, size_t size) const override
  {
    auto parser = std::make_unique<ParserType>(buffer, size, m_importer);
    for (auto &callback : m_callbacks)
    {
      parser->addOnNewNodeCallback(callback);
    }
    parser->withParameters(m_parameters);
    return parser;
  }

  ParserBuilder& withOnNewNodeCallbacks(const std::vector<NewNodeCallback> &callbacks) override
  {
    m_callbacks = callbacks;
    return *this;
  }

  ParserBuilder& withImporter(const Importer& inImporter) override
  {
    m_importer = &inImporter;
    return *this;
  }

  ParserBuilder& withParameters(const ParserParameters &inParameter) override
  {
    m_parameters += inParameter;
    return *this;
  }

private:
  std::vector<NewNodeCallback> m_callbacks;
  const Importer* m_importer;
  ParserParameters m_parameters;
};
} // namespace docwire
#endif //DOCWIRE_PARSER_WRAPPER_H
