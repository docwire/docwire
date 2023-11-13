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

#include "csv_writer.h"
#include "parser.h"

namespace docwire
{

namespace experimental
{

void
CsvWriter::write_to(const Info &info, std::ostream &stream)
{
  if (!m_in_table && info.tag_name != StandardTag::TAG_TABLE)
    return;
  if (info.tag_name == StandardTag::TAG_TABLE)
    m_in_table = true;
  else if (info.tag_name == StandardTag::TAG_CLOSE_TABLE)
    m_in_table = false;
  else if (info.tag_name == StandardTag::TAG_CLOSE_TR)
  {
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
  }
  else if (info.tag_name == StandardTag::TAG_CLOSE_TD)
  {
    m_curr_line.push_back(m_curr_cell);
    m_curr_cell = "";
  }
  else if (info.tag_name == StandardTag::TAG_TEXT)
    m_curr_cell += info.plain_text;
}

Writer*
CsvWriter::clone() const
{
  return new CsvWriter(*this);
}

} // namespace experimental

} // namespace docwire
