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
#include <chrono>
#include <cassert>
#include <sstream>

#include "exception.h"
#include "plain_text_writer.h"
#include "misc.h"

namespace docwire
{
class Cell
{
public:
  Cell()
  : writer()
  {}

  void write(const std::string& s)
  {
    result += s;
    std::stringstream ss(result);
    lines.clear();
    std::string line;
    while (std::getline(ss, line, '\n'))
    {
      lines.push_back(line);
    }
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

struct PlainTextWriter::Implementation
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
      text += " creation time: " + timestampToString(*mail.date) + "\n";
    }
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_attachment(const tag::Attachment& attachment)
  {
    std::string text = "attachment: \n\n";
    if (attachment.name)
    {
      text += "name: " + *attachment.name + "\n";
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
      text += *folder.name + "\n";
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
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_close_attachment(const tag::CloseAttachment&)
  {
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_new_line(const tag::BreakLine&)
  {
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_new_paragraph(const tag::CloseParagraph&)
  {
    if (list_mode)
    {
      return std::make_shared<TextElement>("");
    }
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_link(const tag::Link& link)
  {
    if (link.url)
    {
      return std::make_shared<TextElement>("<" + *link.url + ">");
    }

    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_image(const tag::Image& image)
  {
    if (image.alt)
    {
      return std::make_shared<TextElement>(*image.alt);
    }
    return std::make_shared<TextElement>("");
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
    return std::make_shared<TextElement>("\n");
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
    return std::make_shared<TextElement>("\n");
  }

	std::shared_ptr<TextElement>
	write_comment(const tag::Comment& comment)
	{
		std::string text = "\n[[[";
		if (comment.author)
		{
			text += "COMMENT BY " + *comment.author;
		}
		if (comment.time)
		{
			text += " (" + *comment.time + ")";
		}
		text += "]]]\n";
		if (comment.comment)
		{
			auto comment_text = *comment.comment;
			text += comment_text;
			if (comment_text.empty() || *comment_text.rbegin() != '\n')
				text += "\n";
		}
		text += "[[[---]]]\n";

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
		return std::make_shared<TextElement>("\n");
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

	std::shared_ptr<TextElement> write_close_document(const tag::CloseDocument&)
	{
		std::string footer = footer_stream.str();
		if (!footer.empty())
			footer +=  '\n';
		return std::make_shared<TextElement>("\n" + footer);
	}

  Implementation()
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
          row_result += row[j].getLine(i);
          int size_diff = max_column_width - row[j].getLine(i).size();
          int right_margin = j < (row.size() - 1) ? 2 : 0;
          row_result += add_shift(size_diff + right_margin);
        }
        row_result += "\n";
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
        PlainTextWriter writer;
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
        table.back().back().write(ss.str());
      }
      else if (std::holds_alternative<tag::TableRow>(tags[i]))
      {
        table.push_back({});
      }
      else if (std::holds_alternative<tag::TableCell>(tags[i]))
      {
        table.back().push_back(Cell());
      }
      else if (!std::holds_alternative<tag::CloseTableRow>(tags[i]) && !std::holds_alternative<tag::CloseTableCell>(tags[i]))
      {
        if (table.empty())
          throw LogicError("Cell content in table without rows.");
         if (table.back().empty())
          throw LogicError("Cell content in table row without cells.");
        table.back().back().write(tags[i]);
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
          [this](const tag::Link& tag){return write_link(tag);},
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
          [this](const tag::CloseDocument& tag){return write_close_document(tag);},
          [](const auto&) {return std::shared_ptr<TextElement>{};}
        },
	      tag
      );
      if (text_element)
        text_element->write_to(footer_mode ? footer_stream : stream);
    }
  }

  int level { 0 };
  std::vector<Tag> tags;
  std::string list_type;
  int list_counter;
  bool first_cell_in_row;
  bool list_mode{ false };
  bool header_mode{false};
  bool footer_mode{false};
  std::stringstream footer_stream;
  std::vector<std::vector<Cell>> table;
  std::string footer;
};

PlainTextWriter::PlainTextWriter()
{
  impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{}, ImplementationDeleter{}};
}

PlainTextWriter::PlainTextWriter(const PlainTextWriter &plainTextWriter)
{
  impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{}, ImplementationDeleter{}};
  impl->level = plainTextWriter.impl->level;
  impl->tags = plainTextWriter.impl->tags;
  impl->list_counter = plainTextWriter.impl->list_counter;
  impl->first_cell_in_row = plainTextWriter.impl->first_cell_in_row;
  impl->list_mode = plainTextWriter.impl->list_mode;
  impl->header_mode = plainTextWriter.impl->header_mode;
  impl->footer_mode = plainTextWriter.impl->footer_mode;
  impl->table = plainTextWriter.impl->table;
}

PlainTextWriter&
PlainTextWriter::operator=(const PlainTextWriter &plainTextWriter)
{
  impl->level = plainTextWriter.impl->level;
  impl->tags = plainTextWriter.impl->tags;
  impl->list_counter = plainTextWriter.impl->list_counter;
  impl->first_cell_in_row = plainTextWriter.impl->first_cell_in_row;
  impl->list_mode = plainTextWriter.impl->list_mode;
  impl->header_mode = plainTextWriter.impl->header_mode;
  impl->footer_mode = plainTextWriter.impl->footer_mode;
  impl->table = plainTextWriter.impl->table;

  return *this;
}

void
PlainTextWriter::ImplementationDeleter::operator()(Implementation *impl)
{
  delete impl;
}

void
PlainTextWriter::write_to(const Tag& tag, std::ostream &stream)
{
  impl->write_to(tag, stream);
}

Writer*
PlainTextWriter::clone() const
{
  return new PlainTextWriter();
}

} // namespace docwire
