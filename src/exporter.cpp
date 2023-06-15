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
#include "html_writer.h"
#include "csv_writer.h"
#include "plain_text_writer.h"
#include "meta_data_writer.h"
#include "exporter.h"

namespace doctotext
{

class Exporter::Implementation
{
public:
  Implementation(std::unique_ptr<Writer> writer)
    : _output(nullptr),
      _writer(std::move(writer))
  {}

  Implementation(std::unique_ptr<Writer> writer, std::ostream &out_stream)
    : _output(&out_stream),
      _writer(std::move(writer))
  {}

  Implementation(const Implementation &other)
    : _output(other._output),
      _writer(other._writer->clone())
  {}

  Implementation(const Implementation &&other)
    : _output(other._output),
      _writer(other._writer->clone())
  {}

  std::unique_ptr<Writer> _writer;
  std::ostream *_output;

  bool is_valid() const
  {
    return _output != nullptr;
  }

  void
  set_out_stream(std::ostream &out_stream)
  {
    _output = &out_stream;
  }

  void
  export_to(doctotext::Info &info) const
  {
    _writer->write_to(info, *_output);
  }

  std::ostream &
  get_output() const
  {
    return *_output;
  }
};

Exporter::Exporter(std::unique_ptr<Writer> writer)
{
  impl = std::unique_ptr<Implementation>{new Implementation{std::move(writer)}};
}

Exporter::Exporter(std::unique_ptr<Writer> writer, std::ostream &out_stream)
{
  impl = std::unique_ptr<Implementation>{new Implementation{std::move(writer), out_stream}};
}

Exporter::Exporter(const Exporter &other)
  : impl(new Implementation(*other.impl))
{}

Exporter::Exporter(const Exporter &&other)
  : impl(new Implementation(*other.impl))
{}

Exporter::~Exporter()
{
}

bool
Exporter::is_valid() const
{
  return impl->is_valid();
}

void
Exporter::process(doctotext::Info &info) const
{
  impl->export_to(info);
}

void
Exporter::set_out_stream(std::ostream &out_stream)
{
  impl->set_out_stream(out_stream);
}

void
Exporter::export_to(doctotext::Info &info) const
{
  impl->export_to(info);
}

std::ostream &
Exporter::get_output() const
{
  return impl->get_output();
}

HtmlExporter::HtmlExporter(OriginalAttributesMode original_attributes_mode)
  : Exporter(std::make_unique<HtmlWriter>(static_cast<HtmlWriter::OriginalAttributesMode>(original_attributes_mode)))
{}

HtmlExporter::HtmlExporter(std::ostream &out_stream, OriginalAttributesMode original_attributes_mode)
: Exporter(std::make_unique<HtmlWriter>(static_cast<HtmlWriter::OriginalAttributesMode>(original_attributes_mode)), out_stream)
{}

PlainTextExporter::PlainTextExporter()
  : Exporter(std::make_unique<PlainTextWriter>())
{}

PlainTextExporter::PlainTextExporter(std::ostream &out_stream)
: Exporter(std::make_unique<PlainTextWriter>(), out_stream)
{}

PlainTextExporter::PlainTextExporter(std::ostream &&out_stream)
: Exporter(std::make_unique<PlainTextWriter>(), out_stream)
{}

namespace experimental
{

CsvExporter::CsvExporter()
  : Exporter(std::make_unique<CsvWriter>())
{}

CsvExporter::CsvExporter(std::ostream &out_stream)
: Exporter(std::make_unique<CsvWriter>(), out_stream)
{}

} // namespace experimental

MetaDataExporter::MetaDataExporter()
  : Exporter(std::make_unique<MetaDataWriter>())
{}

MetaDataExporter::MetaDataExporter(std::ostream &out_stream)
: Exporter(std::make_unique<MetaDataWriter>(), out_stream)
{}

} // namespace doctotext
