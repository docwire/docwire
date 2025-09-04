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

#ifndef DOCWIRE_CSV_WRITER_H
#define DOCWIRE_CSV_WRITER_H

#include "core_export.h"
#include <iostream>
#include "writer.h"
#include <vector>

namespace docwire
{

class DOCWIRE_CORE_EXPORT CsvWriter : public Writer
{
public:
  /**
   * @brief Converts text from callback to CSV format
   * @param msg data from callback
   * @param stream output stream
   */
  void write_to(const message_ptr& msg, std::ostream &stream) override;

private:
  bool m_in_table { false };
  std::vector<std::string> m_curr_line;
  std::string m_curr_cell;
};

} // namespace docwire

#endif //DOCWIRE_CSV_WRITER_H
