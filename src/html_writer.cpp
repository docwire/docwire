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

#include <memory>
#include "html_writer.h"
#include <map>
#include "misc.h"
#include <numeric>
#include <sstream>

namespace docwire
{

namespace
{

std::string to_space_separated(const std::vector<std::string>& v)
{
  return v.empty() ? std::string{} :
    std::accumulate(v.begin()+1, v.end(), v.front(),
                    [](const std::string& s, const auto& i) { return s + " " + i; });
}

using HtmlAttrs = std::map<std::string, std::string>;

HtmlAttrs styling_attributes(const attributes::Styling& styling)
{
  HtmlAttrs attrs;
  if (!styling.classes.empty())
    attrs.insert({"class", to_space_separated(styling.classes)});
  if (!styling.id.empty())
    attrs.insert({"id", styling.id});
  if (!styling.style.empty())
    attrs.insert({"style", styling.style});
  return attrs;
}

template<attributes::WithStyling T>
HtmlAttrs styling_attributes(const T& tag)
{
  return styling_attributes(tag.styling);
}

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

std::string to_string(const HtmlAttrs& attrs)
{
  return std::accumulate(attrs.begin(), attrs.end(), std::string{},
    [](const std::string &attrs_string, const auto &a){return attrs_string + " " + a.first + "=\"" + encoded(a.second) + "\""; });
}

std::shared_ptr<TextElement> tag_with_attributes(const std::string& tag_name, const HtmlAttrs& attributes)
{
  return std::make_shared<TextElement>("<" + tag_name + to_string(attributes) + ">");
}

} // anonymous namespace

template<>
struct pimpl_impl<HtmlWriter> : pimpl_impl_base
{
  bool m_header_is_open { false };
  int m_nested_docs_counter { 0 };

  std::shared_ptr<TextElement>
  write_open_header(const tag::Document& document)
  {
    std::string header = {"<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "<meta charset=\"utf-8\">\n"
           "<title>DocWire</title>\n"};
    std::ostringstream meta_str;
    write_metadata(document.metadata())->write_to(meta_str);
    header += meta_str.str();
    m_header_is_open = true;
    return std::make_shared<TextElement>(header);
  }

  std::shared_ptr<TextElement> write_close_header_open_body()
  {
    m_header_is_open = false;
    return std::make_shared<TextElement>("</head>\n<body>\n");
  }

  std::shared_ptr<TextElement>
  write_footer()
  {
    std::string footer = {"</body>\n"
           "</html>\n"};
    return std::make_shared<TextElement>(footer);
  }

  std::shared_ptr<TextElement> write_link(const tag::Link& link)
  {
    HtmlAttrs attrs = styling_attributes(link);
    if (link.url)
      attrs.insert({"href", *link.url });
    return tag_with_attributes("a", attrs);
  }

  std::shared_ptr<TextElement> write_image(const tag::Image& image)
  {
    HtmlAttrs attrs = styling_attributes(image);
    attrs.insert(
    {
      { "src", image.src },
      { "alt", image.alt.value_or("") }
    });
    return tag_with_attributes("img", attrs);
  }

  std::shared_ptr<TextElement> write_list(const tag::List& list)
  {
    HtmlAttrs attrs = styling_attributes(list);
    std::string orig_style = attrs.count("style") ? attrs["style"] + "; " : "";
    std::string list_type = list.type;
    if (list_type != "decimal" && list_type != "disc" && list_type != "none")
      list_type = '"' + list_type + '"';
    attrs.insert_or_assign("style", orig_style + "list-style-type: " + list_type);
    return tag_with_attributes("ul", attrs);
  }

  std::shared_ptr<TextElement> write_style(const tag::Style& style)
  {
    return std::make_shared<TextElement>(
        "<style type=\"text/css\">" + style.css_text + "</style>\n"/* : ""*/);
  }

  std::shared_ptr<TextElement> write_metadata(const attributes::Metadata& metadata)
  {
    std::string meta;
    if (metadata.author)
      meta += "<meta name=\"author\" content=\"" + encoded(*metadata.author) + "\">\n";
    if (metadata.creation_date)
      meta += "<meta name=\"creation-date\" content=\"" + date_to_string(*metadata.creation_date) + "\">\n";
    if (metadata.last_modified_by)
      meta += "<meta name=\"last-modified-by\" content=\"" + encoded(*metadata.last_modified_by) + "\">\n";
    if (metadata.last_modification_date)
      meta += "<meta name=\"last-modification-date\" content=\"" + date_to_string(*metadata.last_modification_date) + "\">\n";
    if (metadata.email_attrs)
    {
      meta += "<meta name=\"from\" content=\"" + encoded(metadata.email_attrs->from) + "\">\n";
      meta += "<meta name=\"date\" content=\"" + date_to_string(metadata.email_attrs->date) + "\">\n";
      meta += "<meta name=\"to\" content=\"" + encoded(*metadata.email_attrs->to) + "\">\n";
      meta += "<meta name=\"subject\" content=\"" + encoded(*metadata.email_attrs->subject) + "\">\n";
      meta += "<meta name=\"reply-to\" content=\"" + encoded(*metadata.email_attrs->reply_to) + "\">\n";
      meta += "<meta name=\"sender\" content=\"" + encoded(*metadata.email_attrs->sender) + "\">\n";
    }
    return std::make_shared<TextElement>(meta);
  }

  void write_to(const Tag& tag, std::ostream &stream)
  {
    if (!std::holds_alternative<tag::Style>(tag) && !std::holds_alternative<tag::Document>(tag) && !std::holds_alternative<tag::CloseDocument>(tag) && m_header_is_open)
      write_close_header_open_body()->write_to(stream);
    std::shared_ptr<TextElement> text_element = std::visit(overloaded {
      [](const tag::Paragraph& tag) { return tag_with_attributes("p", styling_attributes(tag)); },
      [](const tag::CloseParagraph& tag) { return std::make_shared<TextElement>("</p>"); },
      [](const tag::Section& tag) { return tag_with_attributes("div", styling_attributes(tag)); },
      [](const tag::CloseSection& tag) { return std::make_shared<TextElement>("</div>"); },
      [](const tag::Span& tag) { return tag_with_attributes("span", styling_attributes(tag)); },
      [](const tag::CloseSpan& tag) { return std::make_shared<TextElement>("</span>"); },
      [](const tag::Bold& tag) { return tag_with_attributes("b", styling_attributes(tag)); },
      [](const tag::CloseBold& tag) { return std::make_shared<TextElement>("</b>"); },
      [](const tag::Italic& tag) { return tag_with_attributes("i", styling_attributes(tag)); },
      [](const tag::CloseItalic& tag) { return std::make_shared<TextElement>("</i>"); },
      [](const tag::Underline& tag) { return tag_with_attributes("u", styling_attributes(tag)); },
      [](const tag::CloseUnderline& tag) { return std::make_shared<TextElement>("</u>"); },
      [](const tag::Table& tag) { return tag_with_attributes("table", styling_attributes(tag)); },
      [](const tag::CloseTable& tag) { return std::make_shared<TextElement>("</table>"); },
      [](const tag::TableRow& tag) { return tag_with_attributes("tr", styling_attributes(tag)); },
      [](const tag::CloseTableRow& tag) { return std::make_shared<TextElement>("</tr>"); },
      [](const tag::TableCell& tag) { return tag_with_attributes("td", styling_attributes(tag)); },
      [](const tag::CloseTableCell& tag) { return std::make_shared<TextElement>("</td>"); },
      [](const tag::BreakLine& tag) { return tag_with_attributes("br", styling_attributes(tag)); },
      [](const tag::Text& tag) { return std::make_shared<TextElement>(tag.text); },
      [this](const tag::Link& tag) { return write_link(tag); },
      [](const tag::CloseLink& tag) { return std::make_shared<TextElement>("</a>"); },
      [this](const tag::Image& tag) { return write_image(tag); },
      [this](const tag::List& tag) { return write_list(tag); },
      [](const tag::CloseList& tag) { return std::make_shared<TextElement>("</ul>"); },
      [](const tag::ListItem& tag) { return std::make_shared<TextElement>("<li>"); },
      [](const tag::CloseListItem& tag) { return std::make_shared<TextElement>("</li>"); },
      [](const tag::Header& tag) { return std::make_shared<TextElement>("<header>"); },
      [](const tag::CloseHeader& tag) { return std::make_shared<TextElement>("</header>"); },
      [](const tag::Footer& tag) { return std::make_shared<TextElement>("<footer>"); },
      [](const tag::CloseFooter& tag) { return std::make_shared<TextElement>("</footer>"); },
      [this](const tag::Document& tag) { m_nested_docs_counter++; return m_nested_docs_counter == 1 ? write_open_header(tag) : std::shared_ptr<TextElement>(); },
      [this](const tag::CloseDocument& tag) { m_nested_docs_counter--; return m_nested_docs_counter == 0 ? write_footer() : std::shared_ptr<TextElement>(); },
      [this](const tag::Style& tag) { return write_style(tag); },
      [](const auto&) { return std::shared_ptr<TextElement>(); }
    }, tag);
    if (text_element)
      text_element->write_to(stream);
  }
};

HtmlWriter::HtmlWriter()
{
}

HtmlWriter::~HtmlWriter() = default;

HtmlWriter::HtmlWriter(HtmlWriter&&) = default;

void HtmlWriter::write_to(const Tag& tag, std::ostream &stream)
{
	impl().write_to(tag, stream);
}

} // namespace docwire
