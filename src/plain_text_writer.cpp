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

  void write(const Info &info)
  {
    std::stringstream stream;
    writer.write_to(info, stream);
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
  write_mail(const Info &info)
  {
    std::string text = "";
    auto level = info.getAttributeValue<int>("level");
    if (level)
    {
      text = add_tabs(text, *level);
    }
    text += "mail: ";
    auto subject = info.getAttributeValue<std::string>("subject");
    if (subject)
    {
      text += *subject;
    }
    auto date = info.getAttributeValue<unsigned int>("date");
    if (date)
    {
      text += " creation time: " + timestampToString(*date) + "\n";
    }
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_attachment(const Info &info)
  {
    std::string text = "attachment: \n\n";
    auto name = info.getAttributeValue<std::string>("name");
    if (name)
    {
      text += "name: " + *name + "\n";
    }
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_folder(const Info &info)
  {
    auto level = info.getAttributeValue<int>("level");
    std::string text = "";
    if (level)
    {
      text = add_tabs(text, *level);
    }
    text += "folder: ";
    auto name = info.getAttributeValue<std::string>("name");
    if (name)
    {
      text += *name + "\n";
    }
    return std::make_shared<TextElement>(text);
  }

  std::shared_ptr<TextElement>
  write_text(const Info &info)
  {
    return std::make_shared<TextElement>(info.plain_text);
  }

  std::shared_ptr<TextElement>
  write_close_mail_body(const Info &info)
  {
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_close_attachment(const Info &info)
  {
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_new_line(const Info &info)
  {
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_new_paragraph(const Info &info)
  {
    if (list_mode)
    {
      return std::make_shared<TextElement>("");
    }
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_link(const Info &info)
  {
    auto url = info.getAttributeValue<std::string>("url");

    if (url)
    {
      return std::make_shared<TextElement>("<" + *url + ">");
    }

    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_image(const Info &info)
  {
    auto alt = info.getAttributeValue<std::string>("alt");
    if (alt)
    {
      return std::make_shared<TextElement>(*alt);
    }
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  turn_on_table_mode(const Info &info)
  {
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  turn_off_table_mode(const Info &info)
  {
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  write_list(const Info &info)
  {
    list_mode = true;
    list_counter = 1;
    list_type = info.getAttributeValue<std::string>("type").value_or("");
    if (list_type.empty())
    {
      if (info.getAttributeValue<bool>("is_ordered").value_or(false))
        list_type = "decimal";
      else
        list_type = info.getAttributeValue<std::string>("list_style_prefix").value_or("disc");
    }
    return std::make_shared<TextElement>("\n");
  }

  std::shared_ptr<TextElement>
  write_close_list(const Info &info)
  {
    list_mode = false;
    list_counter = 1;
    return std::make_shared<TextElement>("");
  }

  std::shared_ptr<TextElement>
  write_list_item(const Info &info)
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
  write_close_list_item(const Info &info)
  {
    ++list_counter;
    return std::make_shared<TextElement>("\n");
  }

	std::shared_ptr<TextElement>
	write_comment(const Info &info)
	{
		auto author = info.getAttributeValue<std::string>("author");
		auto time = info.getAttributeValue<std::string>("time");
		auto comment = info.getAttributeValue<std::string>("comment");

		std::string text = "\n[[[";
		if (author)
		{
			text += "COMMENT BY " + *author;
		}
		if (time)
		{
			text += " (" + *time + ")";
		}
		text += "]]]\n";
		if (comment)
		{
			auto comment_text = *comment;
			text += comment_text;
			if (comment_text.empty() || *comment_text.rbegin() != '\n')
				text += "\n";
		}
		text += "[[[---]]]\n";

		return std::make_shared<TextElement>(text);
	}

	std::shared_ptr<TextElement> write_header(const Info &info)
	{
		header_mode = true;
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement> write_close_header(const Info &info)
	{
		header_mode = false;
		return std::make_shared<TextElement>("\n");
	}

	std::shared_ptr<TextElement> write_footer(const Info &info)
	{
		footer_mode = true;
		footer_stream.str("");
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement> write_close_footer(const Info &info)
	{
		footer_mode = false;
		return std::make_shared<TextElement>("");
	}

	std::shared_ptr<TextElement> write_close_document(const Info &info)
	{
		std::string footer = footer_stream.str();
		if (!footer.empty())
			footer +=  '\n';
		return std::make_shared<TextElement>("\n" + footer);
	}

  std::map<std::string, std::function<std::shared_ptr<TextElement>(const Info &info)>> plain_text_writers;

  Implementation()
  {
    plain_text_writers =
      {
        {StandardTag::TAG_MAIL, [this](const Info &info){return write_mail(info);}},
        {StandardTag::TAG_ATTACHMENT, [this](const Info &info){return write_attachment(info);}},
        {StandardTag::TAG_FOLDER, [this](const Info &info){return write_folder(info);}},
        {"", [this](const Info &info){return write_text(info);}},
        {StandardTag::TAG_TEXT, [this](const Info &info){return write_text(info);}},
        {StandardTag::TAG_CLOSE_MAIL_BODY, [this](const Info &info){return write_close_mail_body(info);}},
        {StandardTag::TAG_CLOSE_ATTACHMENT, [this](const Info &info){return write_close_attachment(info);}},
        {StandardTag::TAG_BR, [this](const Info &info){return write_new_line(info);}},
        {StandardTag::TAG_CLOSE_P, [this](const Info &info){return write_new_paragraph(info);}},
        {StandardTag::TAG_CLOSE_SECTION, [this](const Info &info){return write_new_paragraph(info);}},
        {StandardTag::TAG_TABLE, [this](const Info &info){return turn_on_table_mode(info);}},
        {StandardTag::TAG_CLOSE_TABLE, [this](const Info &info){return turn_off_table_mode(info);}},
        {StandardTag::TAG_LINK, [this](const Info &info){return write_link(info);}},
        {StandardTag::TAG_IMAGE, [this](const Info &info){return write_image(info);}},
        {StandardTag::TAG_LIST, [this](const Info &info){return write_list(info);}},
        {StandardTag::TAG_CLOSE_LIST, [this](const Info &info){return write_close_list(info);}},
        {StandardTag::TAG_LIST_ITEM, [this](const Info &info){return write_list_item(info);}},
        {StandardTag::TAG_CLOSE_LIST_ITEM, [this](const Info &info){return write_close_list_item(info);}},
        {StandardTag::TAG_HEADER, [this](const Info &info){return write_header(info);}},
        {StandardTag::TAG_CLOSE_HEADER, [this](const Info &info){return write_close_header(info);}},
        {StandardTag::TAG_FOOTER, [this](const Info &info){return write_footer(info);}},
        {StandardTag::TAG_CLOSE_FOOTER, [this](const Info &info){return write_close_footer(info);}},
        {StandardTag::TAG_COMMENT, [this](const Info &info){return write_comment(info);}},
        {StandardTag::TAG_CLOSE_DOCUMENT, [this](const Info &info){return write_close_document(info);}}
      };
  };

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
      if (tags[i].tag_name == StandardTag::TAG_TABLE)
      {
        std::stringstream ss;
        PlainTextWriter writer;
        int open_table_tags = 1;
        writer.write_to(tags[i], ss);
        do
        {
          writer.write_to(tags[++i], ss);
          if (tags[i].tag_name == StandardTag::TAG_TABLE)
            open_table_tags++;
          else if (tags[i].tag_name == StandardTag::TAG_CLOSE_TABLE)
            open_table_tags--;
        }
        while (open_table_tags > 0);
        table.back().back().write(ss.str());
      }
      else if (tags[i].tag_name == StandardTag::TAG_TR)
      {
        table.push_back({});
      }
      else if (tags[i].tag_name == StandardTag::TAG_TD)
      {
        table.back().push_back(Cell());
      }
      else if (tags[i].tag_name != StandardTag::TAG_CLOSE_TR && tags[i].tag_name != StandardTag::TAG_CLOSE_TD)
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

  void write_to(const Info &info, std::ostream &stream)
  {
    if (info.tag_name == StandardTag::TAG_CLOSE_TABLE)
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
      tags.push_back(info);
    }

    if (info.tag_name == StandardTag::TAG_TABLE)
    {
      level++;
    }

    if (level == 0)
    {
      auto writer_iterator = plain_text_writers.find(info.tag_name);
      if (writer_iterator != plain_text_writers.end())
      {
        auto text_element = writer_iterator->second(info);
        text_element->write_to(footer_mode ? footer_stream : stream);
      }
    }
  }

  int level { 0 };
  std::vector<Info> tags;
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
PlainTextWriter::write_to(const Info &info, std::ostream &stream)
{
  impl->write_to(info, stream);
}

Writer*
PlainTextWriter::clone() const
{
  return new PlainTextWriter();
}

} // namespace docwire
