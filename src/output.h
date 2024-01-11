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

#ifndef DOCWIRE_OUTPUT_H
#define DOCWIRE_OUTPUT_H

#include "chain_element.h"
#include "exception.h"

namespace docwire
{

class Writer;

/**
 *  @brief Output class is responsible for saving data from parsing chain to an output stream.
 *  @code
 *  std::ifstream("file.pdf", std::ios_base::in|std::ios_base::binary) | Importer() | PlainTextExporter() | std::cout; // Imports file.pdf and saves it to std::cout as plain text
 *  @endcode
 */
class DllExport Output : public ChainElement
{
public:
  /**
   * @param out_stream Output stream. Parsing chain will be writing to this stream.
   */
  Output(std::ostream &out_stream);

  /**
   * @param out_stream Output stream. Parsing chain will be writing to this stream.
   */
  Output(std::ostream&& out_stream);

  Output(const Output &other);

  Output(const Output &&other);

  virtual ~Output();

  Output* clone() const override
  {
    return new Output(*this);
  }

  bool is_leaf() const override
  {
    return true;
  }

  void process(Info &info) const override;

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

} // namespace docwire

#endif //DOCWIRE_OUTPUT_H
