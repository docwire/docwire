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

#ifndef DOCWIRE_HTML_WRITER_H
#define DOCWIRE_HTML_WRITER_H

#include <iostream>
#include <fstream>

#include "parser.h"
#include "writer.h"
#include "defines.h"

namespace docwire
{
/**
 * @brief The HTMLWriter class
 *
 * This class is used to converts the parsed data from callbacks into HTML format.
 *
 * example:
 * @code
 * ODFOOXMLParser parser("test.docx");
 * HTMLWriter writer;
 * parser.onNewNode([&writer](Info &info) {
 *      writer.write_to(info, std::cout); // convert callback to html ant writes to std::cout
 *      });
 * @endcode
 */
class DllExport HtmlWriter : public Writer
{
public:

  enum class RestoreOriginalAttributes : bool {};

  /**
   * @param restore_original_attributes should original html attributes extracted by html parser be restored
   */
  explicit HtmlWriter(RestoreOriginalAttributes restore_original_attributes = RestoreOriginalAttributes{false});

  HtmlWriter(const HtmlWriter& html_writer);

  /**
   * @brief Converts text from callback to html format
   * @param info data from callback
   * @param stream output stream
   */
  void write_to(const Info &info, std::ostream &stream) override;
  /**
   * @brief creates a new instance of HtmlWriter
   */
  virtual Writer* clone() const override;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> impl;
};
} // namespace docwire

#endif //DOCWIRE_HTML_WRITER_H
