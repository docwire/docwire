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

#include <any>
#include <optional>
#include "misc.h"

#include "meta_data_writer.h"

namespace docwire
{
std::shared_ptr<TextElement>
write_meta_data(const Info &info)
{
  auto attr = info.attributes;
  std::string text = "";
  auto author = info.getAttributeValue<const char*>("author");
  if (author)
  {
    text += "Author: " + std::string(*author) + "\n";
  }
  else
  {
    text += "Author: unidentified\n";
  }
  if (attr.find("creation date") != attr.end())
  {
    std::optional<std::string> str_date = std::nullopt;
    try
    {
      str_date = date_to_string(std::any_cast<const tm&>(attr["creation date"]));
    }
    catch (const std::bad_any_cast& e)
    {}

    if(str_date && *str_date != "")
      text += "Creation time: " + *str_date + "\n";
    else
      text += "Creation time: unidentified\n"; //1. this two cases are technically different
  }
  else
  {
    text += "Creation time: unidentified\n"; //2. this two cases are technically different
  }
  auto last_modified = info.getAttributeValue<const char*>("last modified by");
  if (last_modified)
  {
    text += "Last modified by: " + std::string(*last_modified) + "\n";
  }
  else
  {
    text += "Last modified by: unidentified\n";
  }
  if (attr.find("last modification date") != attr.end())
  {
    std::optional<std::string> str_date = std::nullopt;
    try
    {
      str_date = date_to_string(std::any_cast<const tm&>(attr["last modification date"]));
    }
    catch (const std::bad_any_cast& e)
    {}

    if(str_date && *str_date != "")
      text += "Last modification time: " + *str_date + "\n";
    else
      text += "Last modification time: unidentified\n";
  }
  else
  {
    text += "Last modification time: unidentified\n";
  }
  auto page_count = info.getAttributeValue<size_t>("page count");
  if (page_count)
  {
    text += "Page count: " + std::to_string(*page_count) + "\n";
  }
  else
  {
    text += "Page count: unidentified\n";
  }
  auto word_count = info.getAttributeValue<size_t>("word count");
  if (word_count)
  {
    text += "Word count: " + std::to_string(*word_count) + "\n";
  }
  else
  {
    text += "Word count: unidentified\n";
  }
  return std::make_shared<TextElement>(text);
}

std::map<std::string, std::function<std::shared_ptr<TextElement>(const Info &info)>> meta_data_writers = {
  {StandardTag::TAG_METADATA, &write_meta_data}};

void
MetaDataWriter::write_to(const Info &info, std::ostream &file)
{
  auto writer_iterator = meta_data_writers.find(info.tag_name);
  if (writer_iterator != meta_data_writers.end())
  {
    writer_iterator->second(info)->write_to(file);
  }
}

Writer*
MetaDataWriter::clone() const
{
  return new MetaDataWriter(*this);
}

} // namespace docwire
