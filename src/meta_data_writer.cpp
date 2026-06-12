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

#include <optional>

#include "document_elements.h"
#include "meta_data_writer.h"
#include "convert_chrono.h" // IWYU pragma: keep

namespace docwire
{
std::shared_ptr<text_element>
write_meta_data(const attributes::metadata& metadata)
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
    text += "Creation time: " + convert::to<std::string>(*metadata.creation_date) + "\n";
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
    text += "Last modification time: " + convert::to<std::string>(*metadata.last_modification_date) + "\n";
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
  return std::make_shared<text_element>(text);
}

void
metadata_writer::write_to(const message_ptr& msg, std::ostream &file)
{
  if (msg->is<document::document>())
  {
    write_meta_data(msg->get<document::document>().metadata())->write_to(file);
  }
}

} // namespace docwire
