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

#include "simple_extractor.h"

#include "chain_element.h"
#include "csv_exporter.h"
#include "decompress_archives.h"
#include "html_exporter.h"
#include "importer.h"
#include "input.h"
#include "meta_data_exporter.h"
#include "output.h"
#include "parsing_chain.h"
#include "plain_text_exporter.h"
#include <sstream>
#include "transformer_func.h"

namespace docwire
{

class SimpleExtractor::Implementation
{
public:
  Implementation(const std::string &file_name, const std::string &plugins_path)
    : m_parser_manager(new ParserManager(plugins_path)),
      m_file_name(file_name)
  {}

  Implementation(std::istream& input_stream, const std::string &plugins_path)
    : m_parser_manager(new ParserManager(plugins_path)),
      m_input_stream(&input_stream)
  {}

  template<typename ExtractorType>
  std::string getText() const
  {
    std::stringstream ss;
    ParsingChain chain = DecompressArchives() | Importer(m_parameters, m_parser_manager);

    if (!m_chain_elements.empty())
    {
      std::for_each(m_chain_elements.begin(), m_chain_elements.end(),
                    [&chain](const std::shared_ptr<ChainElement> &chainElement)
                    {
                      chain = chain | (*chainElement);
                    });
    }
    chain | ExtractorType() | Output(ss);
    if (!m_file_name.empty())
    {
      Input(m_file_name) | chain;
    }
    else if(m_input_stream)
    {
      Input(m_input_stream) | chain;
    }
    return ss.str();
  }

  template<typename ExtractorType>
  void parseText(std::ostream &out_stream) const
  {
    ParsingChain chain = DecompressArchives() | Importer(m_parameters, m_parser_manager);

    if (!m_chain_elements.empty())
    {
      std::for_each(m_chain_elements.begin(), m_chain_elements.end(),
                    [&chain](const std::shared_ptr<ChainElement> &chainElement)
                    {
                      chain = chain | (*chainElement);
                    });
    }
    chain | ExtractorType() | Output(out_stream);
    if (!m_file_name.empty())
    {
      Input(m_file_name) | chain;
    }
    else if(m_input_stream)
    {
      Input(m_input_stream) | chain;
    }
  }

  void
  addParameters(const ParserParameters &parameters)
  {
    m_parameters += parameters;
  }

  void
  addChainElement(ChainElement *chainElement)
  {
    m_chain_elements.push_back(std::shared_ptr<ChainElement>(chainElement));
  }

  void
  setFormattingStyle(const FormattingStyle &style)
  {
    m_parameters += ParserParameters("formatting_style", style);
  }

  void
  addCallbackFunction(NewNodeCallback new_code_callback)
  {
    addChainElement(new TransformerFunc(new_code_callback));
  }

  std::string
  getPlainText() const
  {
    return getText<PlainTextExporter>();
  }

  std::string
  getHtmlText() const
  {
    return getText<HtmlExporter>();
  }

  void parseAsPlainText(std::ostream &out_stream) const
  {
    parseText<PlainTextExporter>(out_stream);
  }

  void parseAsHtml(std::ostream &out_stream) const
  {
    parseText<HtmlExporter>(out_stream);
  }

  void parseAsCsv(std::ostream &out_stream) const
  {
    parseText<CsvExporter>(out_stream);
  }

  std::string
  getMetaData() const
  {
    return getText<MetaDataExporter>();
  }

  const std::shared_ptr<ParserManager> m_parser_manager;
  std::vector<std::shared_ptr<ChainElement>> m_chain_elements;
  std::string m_file_name;
  std::istream* m_input_stream;
  ParserParameters m_parameters;
};

SimpleExtractor::SimpleExtractor(const std::string &file_name, const std::string &plugins_path)
{
  impl = std::unique_ptr<Implementation>{new Implementation{file_name, plugins_path}};
}

SimpleExtractor::SimpleExtractor(std::istream &input_stream, const std::string &plugins_path)
{
  impl = std::unique_ptr<Implementation>{new Implementation{input_stream, plugins_path}};
}

SimpleExtractor::~SimpleExtractor()
{
}

void
SimpleExtractor::addParameters(const ParserParameters &parameters)
{
  impl->addParameters(parameters);
}

void
SimpleExtractor::addChainElement(ChainElement *chainElement)
{
  impl->addChainElement(chainElement);
}

void
SimpleExtractor::setFormattingStyle(const FormattingStyle &style)
{
  impl->setFormattingStyle(style);
}

void
SimpleExtractor::addCallbackFunction(const NewNodeCallback& new_code_callback)
{
  impl->addCallbackFunction(new_code_callback);
}

std::string
SimpleExtractor::getPlainText() const
{
  return impl->getPlainText();
}

std::string
SimpleExtractor::getHtmlText() const
{
  return impl->getHtmlText();
}

void
SimpleExtractor::parseAsPlainText(std::ostream &out_stream) const
{
  impl->parseAsPlainText(out_stream);
}

void
SimpleExtractor::parseAsHtml(std::ostream &out_stream) const
{
  impl->parseAsHtml(out_stream);
}

void
SimpleExtractor::parseAsCsv(std::ostream &out_stream) const
{
  impl->parseAsCsv(out_stream);
}

std::string
SimpleExtractor::getMetaData() const
{
  return impl->getMetaData();
}

} // namespace docwire
