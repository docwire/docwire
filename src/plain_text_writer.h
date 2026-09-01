/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_PLAIN_TEXT_WRITER_H
#define DOCWIRE_PLAIN_TEXT_WRITER_H

#include "core_export.h"
#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <vector>
#include <ctime>
#include "type_id.h"

#include "static_flat_map.h"
#include "writer.h"
#include "message.h"
#include "document_elements.h"
#include "mail_elements.h"
#include "error_tags.h"
#include "time_utils.h"
#include "throw_if.h"
#include "plain_text/output_width.h"
#include "plain_text/wrap_lines.h"
#include "plain_text/allocate_columns_auto.h"
#include "plain_text/render_table.h"
#include "plain_text/compute_column_width_ranges.h"

namespace docwire
{
using docwire::plain_text::output_width;

class plain_text_writer : public writer
{
public:
  plain_text_writer(const std::string& eol_sequence,
    std::function<std::string(const document::link&)> format_link_opening,
    std::function<std::string(const document::close_link&)> format_link_closing,
    output_width max_output_width = output_width{80});

  /**
   * @brief Converts text from callback to plain text format.
   * @param msg data from callback
   * @param stream output stream
   */
  void write_to(const message_ptr& msg, std::ostream &stream) override;

  const std::string eol_sequence() const;

private:
  struct impl;
  std::unique_ptr<impl> m_impl;
};

struct plain_text_writer::impl
{
  using handler_func = std::function<std::shared_ptr<text_element>(const message_ptr&)>;
  using text_handler_map = docwire::static_flat_map<docwire::type_id, handler_func, 26>;

  class cell
  {
  public:
    cell(std::string eol_sequence,
         std::function<std::string(const document::link&)> format_link_opening,
         std::function<std::string(const document::close_link&)> format_link_closing,
         output_width max_output_width)
      : writer(eol_sequence, format_link_opening, format_link_closing, max_output_width)
    {}

    void write(const std::string& s)
    {
      result += s;
      std::stringstream ss(result);
      lines.clear();
      const std::string eol_sequence = writer.eol_sequence();
      std::string::size_type prev_pos = 0;
      std::string::size_type pos = 0;
      while ((pos = result.find(eol_sequence, prev_pos)) != std::string::npos)
      {
        lines.push_back(result.substr(prev_pos, pos - prev_pos));
        prev_pos = pos + eol_sequence.length();
      }
      if (prev_pos < result.length())
        lines.push_back(result.substr(prev_pos));
    }

    void write(const message_ptr& msg)
    {
      std::stringstream stream;
      writer.write_to(msg, stream);
      write(stream.str());
      stream.str(std::string());
    }

    plain_text_writer writer;
    std::string result;
    std::vector<std::string> lines;
  };

  class nested_writer
  {
  public:
    nested_writer(std::string eol_sequence,
                  std::function<std::string(const document::link&)> format_link_opening,
                  std::function<std::string(const document::close_link&)> format_link_closing,
                  output_width max_output_width)
      : m_writer(eol_sequence, format_link_opening, format_link_closing, max_output_width)
    {}

    void write(const message_ptr& msg)
    {
      m_writer.write_to(msg, m_stream);
    }

    std::string result_text() const
    {
      return m_stream.str();
    }

    plain_text_writer m_writer;
    std::stringstream m_stream;
  };

  impl(const std::string& eol_sequence,
       std::function<std::string(const document::link&)> format_link_opening,
       std::function<std::string(const document::close_link&)> format_link_closing,
       output_width max_output_width)
    : m_handlers(std::to_array<text_handler_map::value_type>({
        {type_id_of<mail::mail>(), [this](const message_ptr& msg) { return write_mail(msg->get<mail::mail>()); }},
        {type_id_of<mail::attachment>(), [this](const message_ptr& msg) { return write_attachment(msg->get<mail::attachment>()); }},
        {type_id_of<mail::folder>(), [this](const message_ptr& msg) { return write_folder(msg->get<mail::folder>()); }},
        {type_id_of<document::text>(), [this](const message_ptr& msg) { return write_text(msg->get<document::text>()); }},
        {type_id_of<mail::close_mail_body>(), [this](const message_ptr& msg) { return write_close_mail_body(msg->get<mail::close_mail_body>()); }},
        {type_id_of<mail::close_attachment>(), [this](const message_ptr& msg) { return write_close_attachment(msg->get<mail::close_attachment>()); }},
        {type_id_of<document::break_line>(), [this](const message_ptr& msg) { return write_new_line(msg->get<document::break_line>()); }},
        {type_id_of<document::close_paragraph>(), [this](const message_ptr& msg) { return write_new_paragraph(msg->get<document::close_paragraph>()); }},
        {type_id_of<document::close_section>(), [this](const message_ptr& msg) { return write_new_paragraph(document::close_paragraph()); }},
        {type_id_of<document::table>(), [this](const message_ptr& msg) { return turn_on_table_mode(msg->get<document::table>()); }},
        {type_id_of<document::close_table>(), [this](const message_ptr& msg) { return turn_off_table_mode(msg->get<document::close_table>()); }},
        {type_id_of<document::link>(), [this](const message_ptr& msg) { return std::make_shared<text_element>(m_format_link_opening(msg->get<document::link>())); }},
        {type_id_of<document::close_link>(), [this](const message_ptr& msg) { return std::make_shared<text_element>(m_format_link_closing(msg->get<document::close_link>())); }},
        {type_id_of<document::image>(), [this](const message_ptr& msg) { return write_image(msg->get<document::image>()); }},
        {type_id_of<document::list>(), [this](const message_ptr& msg) { return write_list(msg->get<document::list>()); }},
        {type_id_of<document::close_list>(), [this](const message_ptr& msg) { return write_close_list(msg->get<document::close_list>()); }},
        {type_id_of<document::list_item>(), [this](const message_ptr& msg) { return write_list_item(msg->get<document::list_item>()); }},
        {type_id_of<document::close_list_item>(), [this](const message_ptr& msg) { return write_close_list_item(msg->get<document::close_list_item>()); }},
        {type_id_of<document::header>(), [this](const message_ptr& msg) { return write_header(msg->get<document::header>()); }},
        {type_id_of<document::close_header>(), [this](const message_ptr& msg) { return write_close_header(msg->get<document::close_header>()); }},
        {type_id_of<document::footer>(), [this](const message_ptr& msg) { return write_footer(msg->get<document::footer>()); }},
        {type_id_of<document::close_footer>(), [this](const message_ptr& msg) { return write_close_footer(msg->get<document::close_footer>()); }},
        {type_id_of<document::comment>(), [this](const message_ptr& msg) { return write_comment(msg->get<document::comment>()); }},
        {type_id_of<document::close_page>(), [this](const message_ptr& msg) { return write_close_page(msg->get<document::close_page>()); }},
        {type_id_of<document::document>(), [this](const message_ptr& msg) {
            m_nested_docs_counter++;
            return std::shared_ptr<text_element>();
        }},
        {type_id_of<document::close_document>(), [this](const message_ptr& msg) {
            m_nested_docs_counter--;
            return m_nested_docs_counter == 0 ? write_close_document(msg->get<document::close_document>()) : std::shared_ptr<text_element>();
        }},
    })),
    m_eol_sequence(eol_sequence),
    m_format_link_opening(format_link_opening),
    m_format_link_closing(format_link_closing),
    m_max_output_width(max_output_width)
  {}

  std::string timestampToString(unsigned int timestamp)
  {
    std::time_t temp = timestamp;
    struct tm time_buffer;
    std::tm* t = thread_safe_gmtime(&temp, time_buffer);
    std::stringstream stream;
    stream << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
    return stream.str();
  }

  std::string add_tabs(const std::string &in_text, int tab_number)
  {
    std::string new_text = in_text;
    for (int i = 0; i < tab_number; i++)
    {
      new_text += "\t";
    }
    return new_text;
  }

  std::shared_ptr<text_element>
  write_mail(const mail::mail& mail)
  {
    std::string text = "";
    if (mail.level)
    {
      text = add_tabs(text, *mail.level);
    }
    text += "mail: ";
    if (mail.subject)
    {
      text += *mail.subject;
    }
    if (mail.date)
    {
      text += " creation time: " + timestampToString(*mail.date) + m_eol_sequence;
    }
    return std::make_shared<text_element>(text);
  }

  std::shared_ptr<text_element>
  write_attachment(const mail::attachment& attachment)
  {
    std::string text = m_eol_sequence + m_eol_sequence + "attachment: ";
    if (attachment.name)
    {
      text += *attachment.name;
    }
    text += m_eol_sequence + m_eol_sequence;
    return std::make_shared<text_element>(text);
  }

  std::shared_ptr<text_element>
  write_folder(const mail::folder& folder)
  {
    std::string text = "";
    if (folder.level)
    {
      text = add_tabs(text, *folder.level);
    }
    text += "folder: ";
    if (folder.name)
    {
      text += *folder.name + m_eol_sequence;
    }
    return std::make_shared<text_element>(text);
  }

  std::shared_ptr<text_element>
  write_text(const document::text& text)
  {
    return std::make_shared<text_element>(text.text);
  }

  std::shared_ptr<text_element>
  write_close_mail_body(const mail::close_mail_body&)
  {
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_close_attachment(const mail::close_attachment&)
  {
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_new_line(const document::break_line&)
  {
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_new_paragraph(const document::close_paragraph&)
  {
    if (list_mode)
    {
      return std::make_shared<text_element>("");
    }
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_image(const document::image& image)
  {
    std::string text;
    if (image.structured_content_streamer)
    {
      nested_writer writer{m_eol_sequence, m_format_link_opening, m_format_link_closing, m_max_output_width};
      image.structured_content_streamer.value()(
        message_callbacks
        {
          .m_further = [&](message_ptr msg)
          {
            writer.write(msg);
            return continuation::proceed;
          },
          .m_back = [](message_ptr) -> continuation
          {
            return continuation::proceed;
          }
        });
      text = writer.result_text();
    }
    if (text.empty() && image.alt)
    {
      text = *image.alt;
    }
    return std::make_shared<text_element>(text);
  }

  std::shared_ptr<text_element>
  turn_on_table_mode(const document::table&)
  {
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  turn_off_table_mode(const document::close_table&)
  {
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  write_list(const document::list& list)
  {
    list_mode = true;
    list_counter = 1;
    list_type = list.type;
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_close_list(const document::close_list&)
  {
    list_mode = false;
    list_counter = 1;
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  write_list_item(const document::list_item&)
  {
    if (list_type == "none")
      return std::make_shared<text_element>("");
    else if (list_type == "decimal")
      return std::make_shared<text_element>(std::to_string(list_counter) + ". ");
    else if (list_type == "disc")
      return std::make_shared<text_element>("* ");
    else
      return std::make_shared<text_element>(list_type);
  }

  std::shared_ptr<text_element>
  write_close_list_item(const document::close_list_item&)
  {
    ++list_counter;
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_comment(const document::comment& comment)
  {
    std::string text = m_eol_sequence + "[[[";
    if (comment.author)
    {
      text += "COMMENT BY " + *comment.author;
    }
    if (comment.time)
    {
      text += " (" + *comment.time + ")";
    }
    text += "]]]" + m_eol_sequence;
    if (comment.comment)
    {
      auto comment_text = *comment.comment;
      text += comment_text;
      if (comment_text.empty() || *comment_text.rbegin() != '\n')
        text += m_eol_sequence;
    }
    text += "[[[---]]]" + m_eol_sequence;

    return std::make_shared<text_element>(text);
  }

  std::shared_ptr<text_element> write_header(const document::header&)
  {
    header_mode = true;
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element> write_close_header(const document::close_header&)
  {
    header_mode = false;
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element> write_footer(const document::footer&)
  {
    footer_mode = true;
    footer_stream.str("");
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element> write_close_footer(const document::close_footer&)
  {
    footer_mode = false;
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  write_close_page(const document::close_page&)
  {
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element> write_close_document(const document::close_document&)
  {
    std::string footer = footer_stream.str();
    if (!footer.empty())
      footer += m_eol_sequence;
    return std::make_shared<text_element>(m_eol_sequence + footer);
  }

  std::string render_table()
  {
    std::string result;
    if (table_caption_writer)
    {
        std::string caption = table_caption_writer->result_text();
        std::vector<std::string> caption_lines;
        std::string::size_type start = 0;
        std::string::size_type pos;
        while ((pos = caption.find(m_eol_sequence, start)) != std::string::npos)
        {
            caption_lines.push_back(caption.substr(start, pos - start));
            start = pos + m_eol_sequence.length();
        }
        if (start < caption.length())
            caption_lines.push_back(caption.substr(start));
        auto wrapped_caption_lines = plain_text::wrap_lines(caption_lines, m_max_output_width.value());
        for (const auto& line : wrapped_caption_lines)
            result += line + m_eol_sequence;
    }
    if (table.empty())
        return result;

    size_t max_cols = 0;
    for (const auto& row : table)
        max_cols = std::max(max_cols, row.size());

    std::vector<std::vector<std::vector<std::string>>> raw_cell_lines;
    raw_cell_lines.reserve(table.size());
    for (const auto& row : table) {
        std::vector<std::vector<std::string>> raw_row;
        raw_row.reserve(row.size());
        for (const auto& c : row)
            raw_row.push_back(c.lines);
        raw_row.resize(max_cols);
        raw_cell_lines.push_back(std::move(raw_row));
    }

    auto [min_widths, desired_widths] = plain_text::compute_column_width_ranges(raw_cell_lines, max_cols);

    const int total_width = m_max_output_width.value();
    const int inter_col_gap = 2;
    auto [col_widths, gap] = plain_text::allocate_columns_auto(min_widths, desired_widths,
                                                               total_width, inter_col_gap);

    std::vector<std::vector<std::vector<std::string>>> wrapped_cell_lines;
    wrapped_cell_lines.reserve(table.size());
    for (size_t r = 0; r < table.size(); ++r) {
        std::vector<std::vector<std::string>> wrapped_row;
        wrapped_row.reserve(max_cols);
        for (size_t j = 0; j < max_cols; ++j) {
            if (j < raw_cell_lines[r].size() && !raw_cell_lines[r][j].empty())
                wrapped_row.push_back(plain_text::wrap_lines(raw_cell_lines[r][j], col_widths[j]));
            else
                wrapped_row.push_back({});
        }
        wrapped_cell_lines.push_back(std::move(wrapped_row));
    }

    result += plain_text::render_table(wrapped_cell_lines, col_widths, gap, m_eol_sequence);
    return result;
  }

  std::string create_table()
  {
    for (unsigned int i = 0; i < msgs.size(); ++i)
    {
      if (msgs[i]->is<document::table>())
      {
        std::stringstream ss;
        plain_text_writer writer{m_eol_sequence, m_format_link_opening, m_format_link_closing, m_max_output_width};
        int open_table_msgs = 1;
        writer.write_to(msgs[i], ss);
        do
        {
          writer.write_to(msgs[++i], ss);
          if (msgs[i]->is<document::table>())
            open_table_msgs++;
          else if (msgs[i]->is<document::close_table>())
            open_table_msgs--;
        }
        while (open_table_msgs > 0);
        DOCWIRE_THROW_IF (table.empty(), "Table inside table without rows", errors::program_logic{});
        DOCWIRE_THROW_IF (table.back().empty(), "Table inside table row without cells", errors::program_logic{});
        table.back().back().write(ss.str());
      }
      else if (msgs[i]->is<document::caption>())
      {
        DOCWIRE_THROW_IF (table_caption_mode, "Table caption inside table caption", errors::program_logic{});
        DOCWIRE_THROW_IF (table_caption_writer, "Second caption inside table", errors::program_logic{});
        table_caption_mode = true;
        table_caption_writer = nested_writer{m_eol_sequence, m_format_link_opening, m_format_link_closing, m_max_output_width};
      }
      else if (msgs[i]->is<document::close_caption>())
      {
        DOCWIRE_THROW_IF (!table_caption_mode, "Close caption outside table caption", errors::program_logic{});
        table_caption_mode = false;
      }
      else if (msgs[i]->is<document::table_row>())
      {
        table.push_back({});
      }
      else if (msgs[i]->is<document::table_cell>())
      {
        DOCWIRE_THROW_IF (table.empty(), "Cell inside table without rows", errors::program_logic{});
        table.back().push_back(cell{m_eol_sequence, m_format_link_opening, m_format_link_closing, m_max_output_width});
      }
      else if (!msgs[i]->is<document::close_table_row>() && !msgs[i]->is<document::close_table_cell>())
      {
        if (table_caption_mode)
        {
          table_caption_writer->write(msgs[i]);
        }
        else
        {
          DOCWIRE_THROW_IF (table.empty(), "Cell content inside table without rows", errors::program_logic{});
          DOCWIRE_THROW_IF (table.back().empty(), "Cell content inside table row without cells", errors::program_logic{});
          table.back().back().write(msgs[i]);
        }
      }
    }
    return render_table();
  }

  void write_to(const message_ptr& msg, std::ostream &stream)
  {
    if (msg->is<document::close_table>())
    {
      level--;

      if (level == 0)
      {
        stream << create_table();
        msgs.clear();
        table.clear();
        table_caption_mode = false;
        table_caption_writer.reset();
        return;
      }
    }

    if (level > 0)
    {
      msgs.push_back(msg);
    }

    if (msg->is<document::table>())
    {
      level++;
    }

    if (level == 0)
    {
      std::shared_ptr<text_element> text_element;
      if (auto* handler = m_handlers.find(msg->object_type_id()))
      {
        text_element = (*handler)(msg);
      }
      if (text_element)
        text_element->write_to(footer_mode ? footer_stream : stream);
    }
  }

  text_handler_map m_handlers;
  std::string m_eol_sequence;
  std::function<std::string(const document::link&)> m_format_link_opening;
  std::function<std::string(const document::close_link&)> m_format_link_closing;
  int level { 0 };
  std::vector<message_ptr> msgs;
  std::string list_type;
  int list_counter{1};
  bool list_mode{ false };
  bool header_mode{false};
  bool footer_mode{false};
  std::stringstream footer_stream;
  std::vector<std::vector<cell>> table;
  std::optional<nested_writer> table_caption_writer;
  bool table_caption_mode{false};
  int m_nested_docs_counter { 0 };
  output_width m_max_output_width;
};

inline plain_text_writer::plain_text_writer(const std::string& eol_sequence,
  std::function<std::string(const document::link&)> format_link_opening,
  std::function<std::string(const document::close_link&)> format_link_closing,
  output_width max_output_width)
  : m_impl(std::make_unique<impl>(eol_sequence, format_link_opening, format_link_closing, max_output_width))
{
}

inline void
plain_text_writer::write_to(const message_ptr& msg, std::ostream &stream)
{
  m_impl->write_to(msg, stream);
}

inline const std::string plain_text_writer::eol_sequence() const
{
  return m_impl->m_eol_sequence;
}

} // namespace docwire

#endif //DOCWIRE_PLAIN_TEXT_WRITER_H
