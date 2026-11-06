/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CSV_WRITER_H
#define DF0_CSV_WRITER_H

#include "core_export.h"
#include <iostream>
#include "writer.h"
#include "document_elements.h"
#include <vector>

namespace df0
{

class csv_writer : public writer
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

inline void csv_writer::write_to(const message_ptr &msg, std::ostream &stream) {
  if (!m_in_table && !msg->is<document::table>())
    return;
  if (msg->is<document::table>())
    m_in_table = true;
  else if (msg->is<document::close_table>())
    m_in_table = false;
  else if (msg->is<document::close_table_row>())
  {
    if (!m_curr_line.empty()) {
      stream << m_curr_line[0];
      for (auto cell = m_curr_line.cbegin() + 1; cell != m_curr_line.cend();
           ++cell) {
        stream << ',' << *cell;
      }
    }
    stream << "\r\n";
    m_curr_line.clear();
  }
  else if (msg->is<document::close_table_cell>())
  {
    m_curr_line.push_back(m_curr_cell);
    m_curr_cell = "";
  }
  else if (msg->is<document::text>())
    m_curr_cell += msg->get<document::text>().text;
}

} // namespace df0

#endif //DF0_CSV_WRITER_H
