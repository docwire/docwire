/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include <numeric>

#include "parser_manager.h"
#include "simple_extractor.h"

using namespace doctotext;

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
    std::shared_ptr<Importer> importer;
    if (!m_file_name.empty())
    {
      importer = std::make_shared<Importer>(m_file_name, m_parameters, m_parser_manager);
    }
    else if(m_input_stream)
    {
      importer = std::make_shared<Importer>(*m_input_stream, m_parameters, m_parser_manager);
    }
    if (!m_transformers.empty())
    {
      auto chain = (*importer) | ExtractorType();
      std::for_each(m_transformers.begin(), m_transformers.end(),
                    [&chain](const std::shared_ptr<Transformer> &transformer)
                    {
                      chain = chain | (*transformer);
                    });
      chain | ss;
    }
    else
    {
      (*importer) | ExtractorType() | ss;
    }
    return ss.str();
  }

  template<typename ExtractorType>
  void parseText(std::ostream &out_stream) const
  {
    std::shared_ptr<Importer> importer;
    if (!m_file_name.empty())
    {
      importer = std::make_shared<Importer>(m_file_name, m_parameters, m_parser_manager);
    }
    else if(m_input_stream)
    {
      importer = std::make_shared<Importer>(*m_input_stream, m_parameters, m_parser_manager);
    }
    if (!m_transformers.empty())
    {
      auto chain = (*importer) | ExtractorType();
      std::for_each(m_transformers.begin(), m_transformers.end(),
                    [&chain](const std::shared_ptr<Transformer> &transformer)
                    {
                      chain = chain | (*transformer);
                    });
      chain | out_stream;
    }
    else
    {
      (*importer) | ExtractorType() | out_stream;
    }
  }

  void
  addParameters(const doctotext::ParserParameters &parameters)
  {
    m_parameters += parameters;
  }

  void
  addTransformer(Transformer *transformer)
  {
    m_transformers.push_back(std::shared_ptr<Transformer>(transformer));
  }

  void
  setFormattingStyle(const FormattingStyle &style)
  {
    m_parameters += doctotext::ParserParameters("formatting_style", style);
  }

  void
  addCallbackFunction(NewNodeCallback new_code_callback)
  {
    addTransformer(new TransformerFunc(new_code_callback));
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

  std::string
  getMetaData() const
  {
    return getText<MetaDataExporter>();
  }

  const std::shared_ptr<ParserManager> m_parser_manager;
  std::vector<std::shared_ptr<Transformer>> m_transformers;
  std::string m_file_name;
  std::istream* m_input_stream;
  doctotext::ParserParameters m_parameters;
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
SimpleExtractor::addParameters(const doctotext::ParserParameters &parameters)
{
  impl->addParameters(parameters);
}

void
SimpleExtractor::addTransformer(Transformer *transformer)
{
  impl->addTransformer(transformer);
}

void
SimpleExtractor::setFormattingStyle(const FormattingStyle &style)
{
  impl->setFormattingStyle(style);
}

void
SimpleExtractor::addCallbackFunction(NewNodeCallback new_code_callback)
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

std::string
SimpleExtractor::getMetaData() const
{
  return impl->getMetaData();
}