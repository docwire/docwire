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

#ifndef DOCWIRE_IMPORTER_H
#define DOCWIRE_IMPORTER_H

#include <algorithm>
#include <memory>

#include "chain_element.h"
#include "exception.h"
#include <optional>
#include "parser.h"
#include "parser_builder.h"
#include "parser_parameters.h"
#include "defines.h"

namespace docwire
{

class DllExport Importer : public ChainElement
{
public:
  /**
   * @param parameters parser parameters
   */
  explicit Importer(const ParserParameters &parameters = ParserParameters());

  Importer(const Importer &other);

  Importer(const Importer &&other);

  Importer& operator=(const Importer &other);

  Importer& operator=(const Importer &&other);

  virtual ~Importer();

  bool is_leaf() const override
  {
    return false;
  }

  /**
   * @brief Sets new input stream to parse
   * @param input_stream new input stream to parse
   */
  void set_input_stream(std::istream &input_stream);

  /**
   * @brief Adds parser parameters.
   * @param parameters parser parameters
   */
  void add_parameters(const ParserParameters &parameters);

  virtual std::optional<ParserBuilder*> findParserByExtension(const std::string &file_name) const = 0;
  virtual std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const = 0;

  DOCWIRE_EXCEPTION_DEFINE(FileNotReadable, RuntimeError);
  DOCWIRE_EXCEPTION_DEFINE(FileNotFound, RuntimeError);
  DOCWIRE_EXCEPTION_DEFINE(ParsingFailed, RuntimeError);
  DOCWIRE_EXCEPTION_DEFINE(UnknownFormat, RuntimeError);

protected:
  /**
   * @brief Starts parsing process.
   */
  void process(Info& info) const override;

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

class ParserProvider;

namespace
{
  template<typename... ProviderTypeNames>
  std::optional<ParserBuilder*> findParserByExtension(const std::string& extension)
  {
    std::unique_ptr<ParserProvider> providers[] = {std::unique_ptr<ParserProvider>(new ProviderTypeNames)...};
    for (auto& provider : providers)
    {
      auto builder = provider->findParserByExtension(extension);
      if (builder)
        return builder;
    }
    return std::nullopt;
  }

  template<typename... ProviderTypeNames>
  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer)
  {
    std::unique_ptr<ParserProvider> providers[] = {std::unique_ptr<ParserProvider>(new ProviderTypeNames)...};
    for (auto& provider : providers)
    {
      auto builder = provider->findParserByData(buffer);
      if (builder)
        return builder;
    }
    return std::nullopt;
  }
} // anonymous namespace

/**
 * @brief This class template is used to import a file and parse it using available parsers.
 * @code
 * Input("file.pdf") | MultiformatParser<BasicParserProvider, MailParserProvider, OcrParserProvider>(parameters) | HtmlExporter() | std::cout; // Imports file.pdf and exports it to std::cout as HTML
 * @endcode
 *
 * @see Parser
 */
template<typename ProviderTypeName, typename... ProviderTypeNames>
class MultiformatParser : public Importer
{
  public:
    MultiformatParser(const ParserParameters &parameters = ParserParameters())
      : Importer(parameters)
    {}

    MultiformatParser<ProviderTypeName, ProviderTypeNames...>* clone() const override
    {
      return new MultiformatParser<ProviderTypeName, ProviderTypeNames...>(*this);
    }

  /**
   * @brief Returns parser builder for given extension type or nullopt if no parser is found.
   * @param file_name file name with extension (e.g. ".txt", ".docx", etc.)
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByExtension(const std::string &file_name) const override
  {
    std::string extension = file_name.substr(file_name.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return docwire::findParserByExtension<ProviderTypeName, ProviderTypeNames...>(extension);
  }

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param buffer buffer of raw data
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const override
  {
    return docwire::findParserByData<ProviderTypeName, ProviderTypeNames...>(buffer);
  }
};


} // namespace docwire

#endif //DOCWIRE_IMPORTER_H
