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

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cassert>
#include <sstream>

#include "plain_text_writer.h"
#include "error_tags.h"
#include "misc.h"
#include "throw_if.h"

namespace docwire
{
class Cell
{
public:
  Cell(std::string eol_sequence,
    std::function<std::string(const tag::Link&)> format_link_opening,
    std::function<std::string(const tag::CloseLink&)> format_link_closing)
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

  void write(const Tag& tag)
  {
    std::stringstream stream;
    writer.write_to(tag, stream);
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

  PlainTextWriter writer;
  std::string result;
  std::vector<std::string> lines;
};

class nested_writer
{
public:
  nested_writer(std::string eol_sequence,
    std::function<std::string(const tag::Link&)> format_link_opening,
    std::function<std::string(const tag::CloseLink&)> format_link_closing)
  : m_writer(eol_sequence, format_link_opening, format_link_closing)
  {}

  void write(const Tag& tag)
  {
    m_writer.write_to(tag, m_stream);
  }

  std::string result_text() const
  {
    return m_stream.str();
  }

  PlainTextWriter m_writer;
  std::stringstream m_stream;
};

template<>
struct pimpl_impl<PlainTextWriter> : pimpl_impl_base
{
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

  std::shared_ptr<TextElement>
  write_mail(const tag::Mail& mail)
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
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_attachment(const tag::Attachment& attachment)
  {
    std::string text = "attachment: " + m_eol_sequence + m_eol_sequence;
    if (attachment.name)
    {
      text += "name: " + *attachment.name + m_eol_sequence;
    }
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_folder(const tag::Folder& folder)
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
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_text(const tag::Text& text)
  {
    return std::make_shared<TextElement>(text.text);
  }

  std::shared_ptr<TextElement>
  write_close_mail_body(const tag::CloseMailBody&)
  {
    return std::make_shared<TextElement>(m_eol_sequence);
  }

  std::shared_ptr<TextElement>
  write_close_attachment(const tag::CloseAttachment&)
  {
    return std::make_shared<TextElement>(m_eol_sequence);
  }

  std::shared_ptr<TextElement>
  write_new_line(const tag::BreakLine&)
  {
    return std::make_shared<TextElement>(m_eol_sequence);
  }

  std::shared_ptr<TextElement>
  write_new_paragraph(const tag::CloseParagraph&)
  {
    if (list_mode)
    {
      return std::make_shared<TextElement>("");
    }
    return std::make_shared<TextElement>(m_eol_sequence);
  }

  std::shared_ptr<TextElement>
  write_image(const tag::Image& image)
  {
    std::string text;
    if (image.structured_content_streamer)
    {
      nested_writer writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};  
      image.structured_content_streamer.value()(
        emission_callbacks
        {
          .further = [&](Tag&& tag)
          {
            writer.write(tag);
            return continuation::proceed;
          },
          .back = [](Tag&&) -> continuation
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
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  turn_on_table_mode(const tag::Table&)
  {
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  turn_off_table_mode(const tag::CloseTable&)
  {
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  write_list(const tag::List& list)
  {
    list_mode = true;
    list_counter = 1;
    list_type = list.type;
    return std::make_shared<TextElement>(m_eol_sequence);
  }

  std::shared_ptr<TextElement>
  write_close_list(const tag::CloseList&)
  {
    list_mode = false;
    list_counter = 1;
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  write_list_item(const tag::ListItem&)
  {
    if (list_type == "none")
      return std::make_shared<TextElement>("");
    else if (list_type == "decimal")
      return std::make_shared<TextElement>(std::to_string(list_counter) + ". ");
    else if (list_type == "disc")
      return std::make_shared<TextElement>("* ");
    else
      return std::make_shared<TextElement>(list_type);
  }

  std::shared_ptr<TextElement>
  write_close_list_item(const tag::CloseListItem&)
  {
    ++list_counter;
    return std::make_shared<TextElement>(m_eol_sequence);
  }

	std::shared_ptr<TextElement>
	write_comment(const tag::Comment& comment)
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

		return std::make_shared<TextElement>(text);
	}

	std::shared_ptr<TextElement> write_header(const tag::Header&)
	{
		header_mode = true;
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement> write_close_header(const tag::CloseHeader&)
	{
		header_mode = false;
		return std::make_shared<TextElement>(m_eol_sequence);
	}

	std::shared_ptr<TextElement> write_footer(const tag::Footer&)
	{
		footer_mode = true;
		footer_stream.str("");
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement> write_close_footer(const tag::CloseFooter&)
	{
		footer_mode = false;
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement>
	write_close_page(const tag::ClosePage&)
	{
		return std::make_shared<TextElement>(m_eol_sequence);
	}

	std::shared_ptr<TextElement> write_close_document(const tag::CloseDocument&)
	{
		std::string footer = footer_stream.str();
		if (!footer.empty())
			footer += m_eol_sequence;
		return std::make_shared<TextElement>(m_eol_sequence + footer);
	}

  pimpl_impl(const std::string& eol_sequence,
      std::function<std::string(const tag::Link&)> format_link_opening,
      std::function<std::string(const tag::CloseLink&)> format_link_closing)
    : m_eol_sequence(eol_sequence),
      m_format_link_opening(format_link_opening),
      m_format_link_closing(format_link_closing)
  {
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
    for (unsigned int i = 0; i < tags.size(); ++i)
    {
      if (std::holds_alternative<tag::Table>(tags[i]))
      {
        std::stringstream ss;
        PlainTextWriter writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};
        int open_table_tags = 1;
        writer.write_to(tags[i], ss);
        do
        {
          writer.write_to(tags[++i], ss);
          if (std::holds_alternative<tag::Table>(tags[i]))
            open_table_tags++;
          else if (std::holds_alternative<tag::CloseTable>(tags[i]))
            open_table_tags--;
        }
        while (open_table_tags > 0);
        throw_if (table.empty(), "Table inside table without rows", errors::program_logic{});
        throw_if (table.back().empty(), "Table inside table row without cells", errors::program_logic{});
        table.back().back().write(ss.str());
      }
      else if (std::holds_alternative<tag::Caption>(tags[i]))
      {
        throw_if (table_caption_mode, "Table caption inside table caption", errors::program_logic{});
        throw_if (table_caption_writer, "Second caption inside table", errors::program_logic{});
        table_caption_mode = true;
        table_caption_writer = nested_writer{m_eol_sequence, m_format_link_opening, m_format_link_closing};
      }
      else if (std::holds_alternative<tag::CloseCaption>(tags[i]))
      {
        throw_if (!table_caption_mode, "Close caption outside table caption", errors::program_logic{});
        table_caption_mode = false;
      }
      else if (std::holds_alternative<tag::TableRow>(tags[i]))
      {
        table.push_back({});
      }
      else if (std::holds_alternative<tag::TableCell>(tags[i]))
      {
        throw_if (table.empty(), "Cell inside table without rows", errors::program_logic{});
        table.back().push_back(Cell{m_eol_sequence, m_format_link_opening, m_format_link_closing});
      }
      else if (!std::holds_alternative<tag::CloseTableRow>(tags[i]) && !std::holds_alternative<tag::CloseTableCell>(tags[i]))
      {
        if (table_caption_mode)
        {
          table_caption_writer->write(tags[i]);
        }
        else
        {
          throw_if (table.empty(), "Cell content inside table without rows", errors::program_logic{});
          throw_if (table.back().empty(), "Cell content inside table row without cells", errors::program_logic{});
          table.back().back().write(tags[i]);
        }
      }
    }
    return render_table();
  }

  void write_to(const Tag& tag, std::ostream &stream)
  {
    if (std::holds_alternative<tag::CloseTable>(tag))
    {
      level--;

      if (level == 0)
      {
        stream << create_table();
        tags.clear();
        table.clear();
        table_caption_mode = false;
        table_caption_writer.reset();
        return;
      }
    }

    if (level > 0)
    {
      tags.push_back(tag);
    }

    if (std::holds_alternative<tag::Table>(tag))
    {
      level++;
    }

    if (level == 0)
    {
      auto text_element = std::visit(
        overloaded
        {
          [this](const tag::Mail& tag){return write_mail(tag);},
          [this](const tag::Attachment& tag){return write_attachment(tag);},
          [this](const tag::Folder& tag){return write_folder(tag);},
          [this](const tag::Text& tag){return write_text(tag);},
          [this](const tag::CloseMailBody& tag){return write_close_mail_body(tag);},
          [this](const tag::CloseAttachment& tag){return write_close_attachment(tag);},
          [this](const tag::BreakLine& tag){return write_new_line(tag);},
          [this](const tag::CloseParagraph& tag){return write_new_paragraph(tag);},
          [this](const tag::CloseSection& tag){return write_new_paragraph(tag::CloseParagraph());},
          [this](const tag::Table& tag){return turn_on_table_mode(tag);},
          [this](const tag::CloseTable& tag){return turn_off_table_mode(tag);},
          [this](const tag::Link& tag){return std::make_shared<TextElement>(m_format_link_opening(tag));},
          [this](const tag::CloseLink& tag){return std::make_shared<TextElement>(m_format_link_closing(tag));},
          [this](const tag::Image& tag){return write_image(tag);},
          [this](const tag::List& tag){return write_list(tag);},
          [this](const tag::CloseList& tag){return write_close_list(tag);},
          [this](const tag::ListItem& tag){return write_list_item(tag);},
          [this](const tag::CloseListItem& tag){return write_close_list_item(tag);},
          [this](const tag::Header& tag){return write_header(tag);},
          [this](const tag::CloseHeader& tag){return write_close_header(tag);},
          [this](const tag::Footer& tag){return write_footer(tag);},
          [this](const tag::CloseFooter& tag){return write_close_footer(tag);},
          [this](const tag::Comment& tag){return write_comment(tag);},
          [this](const tag::ClosePage& tag){return write_close_page(tag);},
          [this](const tag::Document& tag) { m_nested_docs_counter++; return std::shared_ptr<TextElement>(); },
          [this](const tag::CloseDocument& tag) { m_nested_docs_counter--; return m_nested_docs_counter == 0 ? write_close_document(tag) : std::shared_ptr<TextElement>(); },
          [](const auto&) {return std::shared_ptr<TextElement>{};}
        },
	      tag
      );
      if (text_element)
        text_element->write_to(footer_mode ? footer_stream : stream);
    }
  }

  std::string m_eol_sequence;
  std::function<std::string(const tag::Link&)> m_format_link_opening;
  std::function<std::string(const tag::CloseLink&)> m_format_link_closing;
  int level { 0 };
  std::vector<Tag> tags;
  std::string list_type;
  int list_counter;
  bool list_mode{ false };
  bool header_mode{false};
  bool footer_mode{false};
  std::stringstream footer_stream;
  std::vector<std::vector<Cell>> table;
  std::optional<nested_writer> table_caption_writer;
  bool table_caption_mode{false};
  int m_nested_docs_counter { 0 };
};

PlainTextWriter::PlainTextWriter(const std::string& eol_sequence,
  std::function<std::string(const tag::Link&)> format_link_opening,
  std::function<std::string(const tag::CloseLink&)> format_link_closing)
    : with_pimpl<PlainTextWriter>(eol_sequence, format_link_opening, format_link_closing)
{
}

void
PlainTextWriter::write_to(const Tag& tag, std::ostream &stream)
{
  impl().write_to(tag, stream);
}

const std::string PlainTextWriter::eol_sequence() const
{
  return impl().m_eol_sequence;
}

} // namespace docwire
