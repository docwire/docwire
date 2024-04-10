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

#include <any>
#include <optional>
#include "misc.h"

#include "meta_data_writer.h"

namespace docwire
{
std::shared_ptr<TextElement>
write_meta_data(const tag::Metadata& metadata)
{
  std::string text = "";
  auto author = metadata.author;
  if (metadata.author)
  {
    text += "Author: " + *author + "\n";
  }
  else
  {
    text += "Author: unidentified\n";
  }
  if (metadata.creation_date)
  {
    text += "Creation time: " + date_to_string(*metadata.creation_date) + "\n";
  }
  else
  {
    text += "Creation time: unidentified\n";
  }
  if (metadata.last_modified_by)
  {
    text += "Last modified by: " + *metadata.last_modified_by + "\n";
  }
  else
  {
    text += "Last modified by: unidentified\n";
  }
  if (metadata.last_modification_date)
  {
      text += "Last modification time: " + date_to_string(*metadata.last_modification_date) + "\n";
  }
  else
  {
    text += "Last modification time: unidentified\n";
  }
  if (metadata.page_count)
  {
    text += "Page count: " + std::to_string(*metadata.page_count) + "\n";
  }
  else
  {
    text += "Page count: unidentified\n";
  }
  if (metadata.word_count)
  {
    text += "Word count: " + std::to_string(*metadata.word_count) + "\n";
  }
  else
  {
    text += "Word count: unidentified\n";
  }
  return std::make_shared<TextElement>(text);
}

void
MetaDataWriter::write_to(const Tag& tag, std::ostream &file)
{
  if (std::holds_alternative<tag::Metadata>(tag))
  {
    write_meta_data(std::get<tag::Metadata>(tag))->write_to(file);
  }
}

} // namespace docwire
