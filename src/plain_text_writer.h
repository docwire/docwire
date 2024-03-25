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

#ifndef DOCWIRE_PLAIN_TEXT_WRITER_H
#define DOCWIRE_PLAIN_TEXT_WRITER_H

#include <iostream>
#include <fstream>

#include "parser.h"
#include "writer.h"
#include "defines.h"

namespace docwire
{
class DllExport PlainTextWriter : public Writer
{
public:
  PlainTextWriter();
  PlainTextWriter(const PlainTextWriter &plainTextWriter);

  /**
   * @brief Converts text from callback to plain text format.
   * @param info data from callback
   * @param stream output stream
   */
  void write_to(const Tag& tag, std::ostream &stream) override;
  /**
   * @brief Creates a new instance of PlainTextWriter.
   */
  virtual Writer* clone() const override;

  PlainTextWriter& operator=(const PlainTextWriter &plainTextWriter);

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> impl;
};
} // namespace docwire

#endif //DOCWIRE_PLAIN_TEXT_WRITER_H
