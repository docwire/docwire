/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include <any>
#include <optional>
#include "misc.h"

#include "meta_data_writer.h"

namespace doctotext
{
std::shared_ptr<TextElement>
write_meta_data(const doctotext::Info &info)
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

std::map<std::string, std::function<std::shared_ptr<TextElement>(const doctotext::Info &info)>> meta_data_writers = {
  {StandardTag::TAG_METADATA, &write_meta_data}};

void
MetaDataWriter::write_to(const doctotext::Info &info, std::ostream &file)
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

} // namespace doctotext
