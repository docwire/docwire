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

#include <boost/container/flat_map.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <typeindex>

#include "mail_elements.h"
#include "document_elements.h"
#include "log_scope.h"
#include "plain_text_writer.h"
#include "error_tags.h"
#include "misc.h"
#include "serialization_document_elements.h" // IWYU pragma: keep
#include "serialization_mail_elements.h" // IWYU pragma: keep
#include "throw_if.h"

namespace docwire
{

class cell
{
public:
  cell(std::string eol_sequence,
    std::function<std::string(const document::link&)> format_link_opening,
    std::function<std::string(const document::close_link&)> format_link_closing)
  : writer(eol_sequence, format_link_opening, format_link_closing)
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

  int width() const
  {
    int width = 0;
    for (const auto & single_line : lines)
    {
      width = std::max(width, int(single_line.size()));
    }
    return width;
  }

  int height() const
  {
    return lines.size();
  }

  std::string getLine(unsigned int idx) const
  {
    return idx < lines.size() ? lines[idx] : "";
  }

  std::string result_text() const
  {
    return result;
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
    std::function<std::string(const document::close_link&)> format_link_closing)
  : m_writer(eol_sequence, format_link_opening, format_link_closing)
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

template<>
struct pimpl_impl<plain_text_writer> : pimpl_impl_base
{
  using handler_func = std::function<std::shared_ptr<text_element>(const message_ptr&)>;
  const boost::container::flat_map<std::type_index, handler_func> m_handlers;

  pimpl_impl(const std::string& eol_sequence,
      std::function<std::string(const document::link&)> format_link_opening,
      std::function<std::string(const document::close_link&)> format_link_closing)
    : m_handlers{
        {typeid(mail::mail), [this](const message_ptr& msg) { return write_mail(msg->get<mail::mail>()); }},
        {typeid(mail::attachment), [this](const message_ptr& msg) { return write_attachment(msg->get<mail::attachment>()); }},
        {typeid(mail::folder), [this](const message_ptr& msg) { return write_folder(msg->get<mail::folder>()); }},
        {typeid(document::text), [this](const message_ptr& msg) { return write_text(msg->get<document::text>()); }},
        {typeid(mail::close_mail_body), [this](const message_ptr& msg) { return write_close_mail_body(msg->get<mail::close_mail_body>()); }},
        {typeid(mail::close_attachment), [this](const message_ptr& msg) { return write_close_attachment(msg->get<mail::close_attachment>()); }},
        {typeid(document::break_line), [this](const message_ptr& msg) { return write_new_line(msg->get<document::break_line>()); }},
        {typeid(document::close_paragraph), [this](const message_ptr& msg) { return write_new_paragraph(msg->get<document::close_paragraph>()); }},
        {typeid(document::close_section), [this](const message_ptr& msg) { return write_new_paragraph(document::close_paragraph()); }},
        {typeid(document::table), [this](const message_ptr& msg) { return turn_on_table_mode(msg->get<document::table>()); }},
        {typeid(document::close_table), [this](const message_ptr& msg) { return turn_off_table_mode(msg->get<document::close_table>()); }},
        {typeid(document::link), [this](const message_ptr& msg) { return std::make_shared<text_element>(m_format_link_opening(msg->get<document::link>())); }},
        {typeid(document::close_link), [this](const message_ptr& msg) { return std::make_shared<text_element>(m_format_link_closing(msg->get<document::close_link>())); }},
        {typeid(document::image), [this](const message_ptr& msg) { return write_image(msg->get<document::image>()); }},
        {typeid(document::list), [this](const message_ptr& msg) { return write_list(msg->get<document::list>()); }},
        {typeid(document::close_list), [this](const message_ptr& msg) { return write_close_list(msg->get<document::close_list>()); }},
        {typeid(document::list_item), [this](const message_ptr& msg) { return write_list_item(msg->get<document::list_item>()); }},
        {typeid(document::close_list_item), [this](const message_ptr& msg) { return write_close_list_item(msg->get<document::close_list_item>()); }},
        {typeid(document::header), [this](const message_ptr& msg) { return write_header(msg->get<document::header>()); }},
        {typeid(document::close_header), [this](const message_ptr& msg) { return write_close_header(msg->get<document::close_header>()); }},
        {typeid(document::footer), [this](const message_ptr& msg) { return write_footer(msg->get<document::footer>()); }},
        {typeid(document::close_footer), [this](const message_ptr& msg) { return write_close_footer(msg->get<document::close_footer>()); }},
        {typeid(document::comment), [this](const message_ptr& msg) { return write_comment(msg->get<document::comment>()); }},
        {typeid(document::close_page), [this](const message_ptr& msg) { return write_close_page(msg->get<document::close_page>()); }},
        {typeid(document::document), [this](const message_ptr& msg) {
            m_nested_docs_counter++;
            return std::shared_ptr<text_element>();
        }},
        {typeid(document::close_document), [this](const message_ptr& msg) {
            m_nested_docs_counter--;
            return m_nested_docs_counter == 0 ? write_close_document(msg->get<document::close_document>()) : std::shared_ptr<text_element>();
        }},
    },
    m_eol_sequence(eol_sequence),
    m_format_link_opening(format_link_opening),
    m_format_link_closing(format_link_closing)
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
    log_scope(mail);
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
    log_scope(attachment);
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
    log_scope(folder);
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
    log_scope(text);
    return std::make_shared<text_element>(text.text);
  }

  std::shared_ptr<text_element>
  write_close_mail_body(const mail::close_mail_body&)
  {
    log_scope();
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_close_attachment(const mail::close_attachment&)
  {
    log_scope();
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_new_line(const document::break_line&)
  {
    log_scope();
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_new_paragraph(const document::close_paragraph&)
  {
    log_scope();
    if (list_mode)
    {
      return std::make_shared<text_element>("");
    }
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_image(const document::image& image)
  {
    log_scope(image);
    std::string text;
    if (image.structured_content_streamer)
    {
      nested_writer writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};  
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
    log_scope();
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  turn_off_table_mode(const document::close_table&)
  {
    log_scope();
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  write_list(const document::list& list)
  {
    log_scope(list);
    list_mode = true;
    list_counter = 1;
    list_type = list.type;
    return std::make_shared<text_element>(m_eol_sequence);
  }

  std::shared_ptr<text_element>
  write_close_list(const document::close_list&)
  {
    log_scope();
    list_mode = false;
    list_counter = 1;
    return std::make_shared<text_element>("");
  }

  std::shared_ptr<text_element>
  write_list_item(const document::list_item&)
  {
    log_scope();
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
    log_scope();
    ++list_counter;
    return std::make_shared<text_element>(m_eol_sequence);
  }

	std::shared_ptr<text_element>
	write_comment(const document::comment& comment)
	{
		log_scope(comment);
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
		log_scope();
		return std::make_shared<text_element>("");
	}

	std::shared_ptr<text_element> write_close_header(const document::close_header&)
	{
    log_scope();
		header_mode = false;
		return std::make_shared<text_element>(m_eol_sequence);
	}

	std::shared_ptr<text_element> write_footer(const document::footer&)
	{
    log_scope();
		footer_mode = true;
		footer_stream.str("");
		return std::make_shared<text_element>("");
	}

	std::shared_ptr<text_element> write_close_footer(const document::close_footer&)
	{
    log_scope();
		footer_mode = false;
		return std::make_shared<text_element>("");
	}

	std::shared_ptr<text_element>
	write_close_page(const document::close_page&)
	{
		log_scope();
		return std::make_shared<text_element>(m_eol_sequence);
	}

	std::shared_ptr<text_element> write_close_document(const document::close_document&)
	{
		log_scope();
		std::string footer = footer_stream.str();
		if (!footer.empty())
			footer += m_eol_sequence;
		return std::make_shared<text_element>(m_eol_sequence + footer);
	}

  std::string add_shift(int count)
  {
    std::string shift;
    for (int i = 0; i < count; ++i)
    {
      shift += " ";
    }
    return shift;
  }

  std::string render_table()
  {
    std::string result;

    if (table_caption_writer)
      result += table_caption_writer->result_text() + m_eol_sequence;

    int max_column_width = 0;
    int cell_in_row = 0;

    for (const auto &row : table)
    {
      cell_in_row = std::max(cell_in_row, int(row.size()));
      for (const auto &cell : row)
      {
        max_column_width = std::max(max_column_width, cell.width());
      }
    }
    // Limit the maximum column width for performance reasons
    constexpr int column_width_limit = 1000;
    max_column_width = std::min(max_column_width, column_width_limit);

    for (const auto &row : table)
    {
      std::string row_result;
      unsigned int max_row_height = 1; // empty rows or rows with all cells empty should be visible
      for (const auto &cell : row)
      {
        max_row_height = std::max(int(max_row_height), cell.height());
      }
      for (unsigned int i = 0; i < max_row_height; ++i)
      {
        for (unsigned int j = 0; j < row.size(); ++j)
        {
          std::string l = row[j].getLine(i);
          // limit length of l to column_width_limit
          l = l.substr(0, std::min(l.size(), size_t(column_width_limit)));
          row_result += l;
          int size_diff = max_column_width - l.size();
          int right_margin = j < (row.size() - 1) ? 2 : 0;
          row_result += add_shift(size_diff + right_margin);
        }
        row_result += m_eol_sequence;
      }
      result += row_result;
    }
    return result;
  }

  std::string create_table()
  {
    for (unsigned int i = 0; i < msgs.size(); ++i)
    {
      if (msgs[i]->is<document::table>())
      {
        std::stringstream ss;
        plain_text_writer writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};
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
        throw_if (table.empty(), "Table inside table without rows", errors::program_logic{});
        throw_if (table.back().empty(), "Table inside table row without cells", errors::program_logic{});
        table.back().back().write(ss.str());
      }
      else if (msgs[i]->is<document::caption>())
      {
        throw_if (table_caption_mode, "Table caption inside table caption", errors::program_logic{});
        throw_if (table_caption_writer, "Second caption inside table", errors::program_logic{});
        table_caption_mode = true;
        table_caption_writer = nested_writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};
      }
      else if (msgs[i]->is<document::close_caption>())
      {
        throw_if (!table_caption_mode, "Close caption outside table caption", errors::program_logic{});
        table_caption_mode = false;
      }
      else if (msgs[i]->is<document::table_row>())
      {
        table.push_back({});
      }
      else if (msgs[i]->is<document::table_cell>())
      {
        throw_if (table.empty(), "Cell inside table without rows", errors::program_logic{});
        table.back().push_back(cell{m_eol_sequence, m_format_link_opening, m_format_link_closing});
      }
      else if (!msgs[i]->is<document::close_table_row>() && !msgs[i]->is<document::close_table_cell>())
      {
        if (table_caption_mode)
        {
          table_caption_writer->write(msgs[i]);
        }
        else
        {
          throw_if (table.empty(), "Cell content inside table without rows", errors::program_logic{});
          throw_if (table.back().empty(), "Cell content inside table row without cells", errors::program_logic{});
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
      auto it = m_handlers.find(std::type_index(msg->object_type()));
      std::shared_ptr<text_element> text_element;
      if (it != m_handlers.end())
      {
        text_element = it->second(msg);
      }
      if (text_element)
        text_element->write_to(footer_mode ? footer_stream : stream);
    }
  }

  std::string m_eol_sequence;
  std::function<std::string(const document::link&)> m_format_link_opening;
  std::function<std::string(const document::close_link&)> m_format_link_closing;
  int level { 0 };
  std::vector<message_ptr> msgs;
  std::string list_type;
  int list_counter;
  bool list_mode{ false };
  bool header_mode{false};
  bool footer_mode{false};
  std::stringstream footer_stream;
  std::vector<std::vector<cell>> table;
  std::optional<nested_writer> table_caption_writer;
  bool table_caption_mode{false};
  int m_nested_docs_counter { 0 };
};

plain_text_writer::plain_text_writer(const std::string& eol_sequence,
  std::function<std::string(const document::link&)> format_link_opening,
  std::function<std::string(const document::close_link&)> format_link_closing)
    : with_pimpl<plain_text_writer>(eol_sequence, format_link_opening, format_link_closing)
{
}

void
plain_text_writer::write_to(const message_ptr& msg, std::ostream &stream)
{
  impl().write_to(msg, stream);
}

const std::string plain_text_writer::eol_sequence() const
{
  return impl().m_eol_sequence;
}

} // namespace docwire
