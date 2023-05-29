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

#include "importer.h"
#include "exporter.h"
#include "transformer.h"
#include "parsing_chain.h"

namespace doctotext
{

class ParsingChain::Implementation
{
public:
  Implementation(const Importer &importer, const Exporter &exporter)
  : m_importer(importer),
    m_exporter(std::shared_ptr<Exporter>(exporter.clone()))
  {
    make_connections();
  }

  Implementation(const Implementation &other)
    : m_importer(other.m_importer),
      m_exporter(std::shared_ptr<Exporter>(other.m_exporter->clone())),
      m_transformers(other.m_transformers)
  {
    make_connections();
  }

  Implementation(const Implementation &&other)
    : m_importer(other.m_importer),
      m_exporter(std::shared_ptr<Exporter>(other.m_exporter->clone())),
      m_transformers(other.m_transformers)
  {
    make_connections();
  }

  Implementation &
  operator=(const Implementation &other)
  {
    m_importer = other.m_importer;
    m_exporter = std::shared_ptr<Exporter>(other.m_exporter->clone());
    m_transformers = other.m_transformers;
    make_connections();
    return *this;
  }

  Implementation &
  operator=(const Implementation &&other)
  {
    m_importer = other.m_importer;
    m_exporter = std::shared_ptr<Exporter>(other.m_exporter->clone());
    m_transformers = other.m_transformers;
    make_connections();
    return *this;
  }

  Importer m_importer;
  std::shared_ptr<Exporter> m_exporter;
  std::vector<std::shared_ptr<Transformer>> m_transformers;

  bool is_valid() const
  {
    bool r = m_importer.is_valid() && m_exporter->is_valid();
    return r;
  }

  void make_connections()
  {
    m_importer.add_callback([this](doctotext::Info &info)
                            {
                              for (auto &transformer: m_transformers) {
                                if (!info.skip) {
                                  transformer->transform(info);
                                }
                              }
                              if (!info.skip) {
                                m_exporter->export_to(info);
                              }
                            });
  }

  void
  process() const
  {
    m_exporter->begin();
    m_importer.process();
    m_exporter->end();
  }
};

ParsingChain::ParsingChain(const Importer &importer, const Exporter &exporter)
  : impl(new Implementation(importer, exporter))
{
  if (impl->is_valid())
  {
    impl->process();
  }
}

ParsingChain::ParsingChain(const ParsingChain &other)
  : impl(new Implementation(*other.impl))
{
}

ParsingChain::ParsingChain(const ParsingChain &&other)
  : impl(new Implementation(*other.impl))
{
}

ParsingChain::~ParsingChain()
{
}

ParsingChain &
ParsingChain::operator=(const ParsingChain &other)
{
  if (this != &other)
  {
    *impl = *other.impl;
  }
  return *this;
}

ParsingChain &
ParsingChain::operator=(const ParsingChain &&other)
{
  if (this != &other)
  {
    *impl = *other.impl;
  }
  return *this;
}

ParsingChain
operator|(std::istream &input_stream, ParsingChain &&parsing_process)
{
  parsing_process.impl->m_importer.set_input_stream(input_stream);
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(std::istream &input_stream, ParsingChain &parsing_process)
{
  parsing_process.impl->m_importer.set_input_stream(input_stream);
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(std::istream &&input_stream, ParsingChain &&parsing_process)
{
  parsing_process.impl->m_importer.set_input_stream(input_stream);
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(std::istream &&input_stream, ParsingChain &parsing_process)
{
  parsing_process.impl->m_importer.set_input_stream(input_stream);
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, std::ostream &out_stream)
{
  auto new_parsing_process = std::move(parsing_process);
  new_parsing_process.impl->m_exporter->set_out_stream(out_stream);
  if (new_parsing_process.impl->is_valid())
  {
    new_parsing_process.impl->process();
  }
  return new_parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, std::ostream &out_stream)
{
  auto new_parsing_process = parsing_process;
  new_parsing_process.impl->m_exporter->set_out_stream(out_stream);
  if (new_parsing_process.impl->is_valid())
  {
    new_parsing_process.impl->process();
  }
  return new_parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, std::ostream &&out_stream)
{
  auto new_parsing_process = std::move(parsing_process);
  new_parsing_process.impl->m_exporter->set_out_stream(out_stream);
  if (new_parsing_process.impl->is_valid())
  {
    new_parsing_process.impl->process();
  }
  return new_parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, std::ostream &&out_stream)
{
  auto new_parsing_process = parsing_process;
  new_parsing_process.impl->m_exporter->set_out_stream(out_stream);
  if (new_parsing_process.impl->is_valid())
  {
    new_parsing_process.impl->process();
  }
  return new_parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, Transformer &&transformer)
{
  parsing_process.impl->m_transformers.push_back(std::shared_ptr<Transformer>(transformer.clone()));
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, Transformer &transformer)
{
  parsing_process.impl->m_transformers.push_back(std::shared_ptr<Transformer>(transformer.clone()));
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, Transformer &&transformer)
{
  parsing_process.impl->m_transformers.push_back(std::shared_ptr<Transformer>(transformer.clone()));
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, Transformer &transformer)
{
  parsing_process.impl->m_transformers.push_back(std::shared_ptr<Transformer>(transformer.clone()));
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, Exporter &&exporter)
{
  parsing_process.impl->m_exporter = std::shared_ptr<Exporter>(exporter.clone());
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &parsing_process, Exporter &exporter)
{
  parsing_process.impl->m_exporter = std::shared_ptr<Exporter>(exporter.clone());
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, Exporter &&exporter)
{
  parsing_process.impl->m_exporter = std::shared_ptr<Exporter>(exporter.clone());
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain
operator|(ParsingChain &&parsing_process, Exporter &exporter)
{
  parsing_process.impl->m_exporter = std::shared_ptr<Exporter>(exporter.clone());
  if (parsing_process.impl->is_valid())
  {
    parsing_process.impl->process();
  }
  return parsing_process;
}

ParsingChain operator|(std::istream &input_stream, const Importer &importer)
{
  auto new_parsing_process = ParsingChain(importer, PlainTextExporter());
  return input_stream | new_parsing_process;
}

ParsingChain operator|(std::istream &input_stream, const Importer &&importer)
{
  auto new_parsing_process = ParsingChain(importer, PlainTextExporter());
  return input_stream | new_parsing_process;
}

ParsingChain operator|(std::istream &&input_stream, const Importer &importer)
{
  auto new_parsing_process = ParsingChain(importer, PlainTextExporter());
  return input_stream | new_parsing_process;
}

ParsingChain operator|(std::istream &&input_stream, const Importer &&importer)
{
  auto new_parsing_process = ParsingChain(importer, PlainTextExporter());
  return input_stream | new_parsing_process;
}

ParsingChain operator|(const Importer &importer, Exporter &&exporter)
{
  return ParsingChain(importer, exporter);
}

ParsingChain operator|(const Importer &importer, Transformer &&transformer)
{
  auto parsing_process = ParsingChain(importer, PlainTextExporter());
  return parsing_process | std::move(transformer);
}

ParsingChain operator|(const Importer &importer, Transformer &transformer)
{
  auto parsing_process = ParsingChain(importer, PlainTextExporter());
  return parsing_process | transformer;
}

} // namespace doctotext