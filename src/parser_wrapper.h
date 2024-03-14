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

#include "metadata.h"
#include "formatting_style.h"
#include "parser.h"
#include "parser_builder.h"
#include "defines.h"

namespace docwire
{

class Importer;

template<typename ParserType>
class DllExport ParserWrapper : public Parser
{
public:
  explicit ParserWrapper(const std::string& file_name, const Importer* inImporter = nullptr)
  : Parser(inImporter),
    m_parser(ParserType(file_name))
  {}

  ParserWrapper(const char* buffer, size_t size, const Importer* inImporter = nullptr)
  : Parser(inImporter) ,
    m_parser(ParserType(buffer, size))
  {}

  void parse() const override
  {
    sendTag(StandardTag::TAG_TEXT,  m_parser.plainText(getFormattingStyle()));
    sendTag(StandardTag::TAG_METADATA, "", m_parser.metaData().getFieldsAsAny());
  }

  Parser &withParameters(const ParserParameters &parameters) override
  {
    Parser::withParameters(parameters);
    return *this;
  }

  void setImporter(const Importer* inImporter)
  {
    m_importer = inImporter;
  }

private:
  mutable ParserType m_parser;
};

template<typename ParserType>
class DllExport wrapper_parser_creator
{
public:
  static std::unique_ptr<Parser>
  create(const std::string &inFileName, const Importer* inImporter)
  {
    return std::make_unique<ParserWrapper<ParserType>>(inFileName, inImporter);
  }

  static std::unique_ptr<Parser>
  create(const char* buffer, size_t size, const Importer* inImporter)
  {
    return std::make_unique<ParserWrapper<ParserType>>(buffer, size, inImporter);
  }
};

template<typename ParserType>
class DllExport parser_creator
{
public:
  static std::unique_ptr<Parser>
  create(const std::string &inFileName, const Importer* inImporter)
  {
    return std::make_unique<ParserType>(inFileName, inImporter);
  }

  static std::unique_ptr<Parser>
  create(const char* buffer, size_t size, const Importer* inImporter)
  {
    return std::make_unique<ParserType>(buffer, size, inImporter);
  }
};

/**
 * @brief Provides the basic mechanism to build any parser.
 * @tparam ParserCreator type of parser to build
 */
template<typename ParserCreator>
class DllExport ParserBuilderWrapper : public ParserBuilder
{
public:
  ParserBuilderWrapper()
  : m_parser_creator(ParserCreator())
  {

  }

  std::unique_ptr<Parser>
  build(const std::string &inFileName) const override
  {
    auto parser = m_parser_creator.create(inFileName, m_importer);
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
    auto parser =  m_parser_creator.create(buffer, size, m_importer);
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
  ParserCreator m_parser_creator;
  std::vector<NewNodeCallback> m_callbacks;
  const Importer* m_importer;
  ParserParameters m_parameters;
};
} // namespace docwire
#endif //DOCWIRE_PARSER_WRAPPER_H
