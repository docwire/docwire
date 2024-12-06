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

#include "csv_writer.h"

namespace docwire
{

void
CsvWriter::write_to(const Tag& tag, std::ostream &stream)
{
  if (!m_in_table && !std::holds_alternative<tag::Table>(tag))
    return;
  return std::visit(
    overloaded {
      [&](const tag::Table&) {
        m_in_table = true;
      },
      [&](const tag::CloseTable&) {
        m_in_table = false;
      },
      [&](const tag::CloseTableRow&) {
        if (!m_curr_line.empty())
        {
          stream << m_curr_line[0];
          for (auto cell = m_curr_line.cbegin() + 1; cell != m_curr_line.cend(); ++cell)
          {
            stream << ',' << *cell;
          }
        }
        stream << "\r\n";
        m_curr_line.clear();
      },
      [&](const tag::CloseTableCell&) {
        m_curr_line.push_back(m_curr_cell);
        m_curr_cell = "";
      },
      [&](const tag::Text& tag) {
        m_curr_cell += tag.text;
      },
      [&](const auto&) {}
    },
	  tag
  );
}

} // namespace docwire
