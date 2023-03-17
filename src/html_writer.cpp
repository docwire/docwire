/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
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

#include <memory>
#include "html_writer.h"
#include "parser.h"
namespace doctotext
{
using doctotext::StandardTag;

  std::string encoded(const std::string& value)
  {
    std::string encoded;
    encoded.reserve(value.size());
    for (auto& ch: value)
    {
      switch(ch)
      {
        case '&': encoded += "&amp;"; break;
        case '\"': encoded += "&quot;"; break;
        case '\'': encoded += "&apos;"; break;
        case '<': encoded += "&lt;"; break;
        case '>': encoded += "&gt;"; break;
        default: encoded += ch; break;
      }
    }
    return encoded;
  }

  std::shared_ptr<TextElement> write_link(const Info& info)
  {
    auto url_attr = encoded(info.getAttributeValue<std::string>("url").value());
    return std::make_shared<TextElement>("<a href=\"" + url_attr + "\">");
  }

  std::shared_ptr<TextElement> write_image(const Info& info)
  {
    auto src_attr = encoded(info.getAttributeValue<std::string>("src").value());
    auto alt_attr = encoded(info.getAttributeValue<std::string>("alt").value_or(""));
    return std::make_shared<TextElement>("<img src=\"" + src_attr + "\" alt=\"" + alt_attr + "\" />");
  }

  std::shared_ptr<TextElement> write_list(const Info& info)
  {
    std::string list_type = info.getAttributeValue<std::string>("type").value_or("");
    if (list_type.empty())
    {
      if (info.getAttributeValue<bool>("is_ordered").value_or(false))
        list_type = "decimal";
      else
        list_type = info.getAttributeValue<std::string>("list_style_prefix").value_or("disc");
    }
    if (list_type != "decimal" && list_type != "disc" && list_type != "none")
      list_type = encoded('"' + list_type + '"');
    return std::make_shared<TextElement>("<ul style=\"list-style-type: " + list_type + "\">");
  }

std::map<std::string, std::function<std::shared_ptr<TextElement>(const doctotext::Info &info)>> writers = {
  {StandardTag::TAG_P, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<p>"); }},
  {StandardTag::TAG_CLOSE_P, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</p>"); }},
  {StandardTag::TAG_SECTION, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<div>"); }},
  {StandardTag::TAG_CLOSE_SECTION, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</div>"); }},
  {StandardTag::TAG_SPAN, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<span>"); }},
  {StandardTag::TAG_CLOSE_SPAN, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</span>"); }},
  {StandardTag::TAG_B, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<b>"); }},
  {StandardTag::TAG_CLOSE_B, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</b>"); }},
  {StandardTag::TAG_I, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<i>"); }},
  {StandardTag::TAG_CLOSE_I, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</i>"); }},
  {StandardTag::TAG_U, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<u>"); }},
  {StandardTag::TAG_CLOSE_U, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</u>"); }},
  {StandardTag::TAG_TABLE, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<table>"); }},
  {StandardTag::TAG_CLOSE_TABLE, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</table>"); }},
  {StandardTag::TAG_TR, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<tr>"); }},
  {StandardTag::TAG_CLOSE_TR, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</tr>"); }},
  {StandardTag::TAG_TD, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<td>"); }},
  {StandardTag::TAG_CLOSE_TD, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</td>"); }},
  {StandardTag::TAG_BR, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<br />"); }},
  {StandardTag::TAG_TEXT, [](const doctotext::Info &info) { return std::make_shared<TextElement>(info.plain_text); }},
  {StandardTag::TAG_LINK, [](const doctotext::Info &info) { return write_link(info); }},
  {StandardTag::TAG_CLOSE_LINK, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</a>"); }},
  {StandardTag::TAG_IMAGE, [](const doctotext::Info &info) { return write_image(info); }},
  {StandardTag::TAG_LIST, [](const doctotext::Info &info) { return write_list(info); }},
  {StandardTag::TAG_CLOSE_LIST, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</ul>"); }},
  {StandardTag::TAG_LIST_ITEM, [](const doctotext::Info &info) { return std::make_shared<TextElement>("<li>"); }},
  {StandardTag::TAG_CLOSE_LIST_ITEM, [](const doctotext::Info &info) { return std::make_shared<TextElement>("</li>"); }}};

void
HtmlWriter::write_header(std::ostream &stream) const
{
  stream << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "<meta charset=\"utf-8\">\n"
         << "<title>DocToText</title>\n"
         << "</head>\n"
         << "<body>\n";
}

void
HtmlWriter::write_footer(std::ostream &stream) const
{
  stream << "</body>\n"
         << "</html>\n";
}

void
HtmlWriter::write_to(const doctotext::Info &info, std::ostream &stream)
{
  auto writer_iterator = writers.find(info.tag_name);
  if (writer_iterator != writers.end())
  {
    writer_iterator->second(info)->write_to(stream);
  }
}

Writer*
HtmlWriter::clone() const
{
return new HtmlWriter(*this);
}
} // namespace doctotext








