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

#ifndef DOCWIRE_CSV_WRITER_H
#define DOCWIRE_CSV_WRITER_H

#include "defines.h"
#include <iostream>
#include "writer.h"
#include <vector>

namespace docwire
{
  struct Info;

namespace experimental
{

/**
 * @brief The CsvWriter class
 *
 * This class is used to converts the parsed data from callbacks into CSV format.
 *
 * example:
 * @code
 * ODFOOXMLParser parser("test.docx");
 * CsvWriter writer;
 * parser.onNewNode([&writer](Info &info) {
 *      writer.write_to(info, std::cout); // convert callback to csv and writes to std::cout
 *      });
 * @endcode
 */
class DllExport CsvWriter : public Writer
{
public:
  /**
   * @brief Converts text from callback to CSV format
   * @param info data from callback
   * @param stream output stream
   */
  void write_to(const Info &info, std::ostream &stream) override;
  /**
   * @brief creates a new instance of HtmlWriter
   */
  virtual Writer* clone() const override;

private:
  bool m_in_table { false };
  std::vector<std::string> m_curr_line;
  std::string m_curr_cell;
};

} // namespace experimental

} // namespace docwire

#endif //DOCWIRE_CSV_WRITER_H
