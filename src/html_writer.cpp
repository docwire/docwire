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
#include "base64.h"
#include <boost/container/flat_map.hpp>
#include "error_tags.h"
#include "misc.h"
#include "throw_if.h"
#include <numeric>
#include "document_elements.h"
#include <sstream>
#include <typeindex>
#include <functional>

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
  using handler_func = std::function<std::shared_ptr<TextElement>(const message_ptr&)>;
  const boost::container::flat_map<std::type_index, handler_func> m_handlers;

  pimpl_impl()
    : m_handlers{
        {typeid(document::Paragraph), [](const message_ptr& msg) { return tag_with_attributes("p", styling_attributes(msg->get<document::Paragraph>())); }},
        {typeid(document::CloseParagraph), [](const message_ptr&) { return std::make_shared<TextElement>("</p>"); }},
        {typeid(document::Section), [](const message_ptr& msg) { return tag_with_attributes("div", styling_attributes(msg->get<document::Section>())); }},
        {typeid(document::CloseSection), [](const message_ptr&) { return std::make_shared<TextElement>("</div>"); }},
        {typeid(document::Span), [](const message_ptr& msg) { return tag_with_attributes("span", styling_attributes(msg->get<document::Span>())); }},
        {typeid(document::CloseSpan), [](const message_ptr&) { return std::make_shared<TextElement>("</span>"); }},
        {typeid(document::Bold), [](const message_ptr& msg) { return tag_with_attributes("b", styling_attributes(msg->get<document::Bold>())); }},
        {typeid(document::CloseBold), [](const message_ptr&) { return std::make_shared<TextElement>("</b>"); }},
        {typeid(document::Italic), [](const message_ptr& msg) { return tag_with_attributes("i", styling_attributes(msg->get<document::Italic>())); }},
        {typeid(document::CloseItalic), [](const message_ptr&) { return std::make_shared<TextElement>("</i>"); }},
        {typeid(document::Underline), [](const message_ptr& msg) { return tag_with_attributes("u", styling_attributes(msg->get<document::Underline>())); }},
        {typeid(document::CloseUnderline), [](const message_ptr&) { return std::make_shared<TextElement>("</u>"); }},
        {typeid(document::Table), [](const message_ptr& msg) { return tag_with_attributes("table", styling_attributes(msg->get<document::Table>())); }},
        {typeid(document::CloseTable), [](const message_ptr&) { return std::make_shared<TextElement>("</table>"); }},
        {typeid(document::TableRow), [](const message_ptr& msg) { return tag_with_attributes("tr", styling_attributes(msg->get<document::TableRow>())); }},
        {typeid(document::CloseTableRow), [](const message_ptr&) { return std::make_shared<TextElement>("</tr>"); }},
        {typeid(document::TableCell), [](const message_ptr& msg) { return tag_with_attributes("td", styling_attributes(msg->get<document::TableCell>())); }},
        {typeid(document::CloseTableCell), [](const message_ptr&) { return std::make_shared<TextElement>("</td>"); }},
        {typeid(document::Caption), [](const message_ptr& msg) { return tag_with_attributes("caption", styling_attributes(msg->get<document::Caption>())); }},
        {typeid(document::CloseCaption), [](const message_ptr&) { return std::make_shared<TextElement>("</caption>"); }},
        {typeid(document::BreakLine), [](const message_ptr& msg) { return tag_with_attributes("br", styling_attributes(msg->get<document::BreakLine>())); }},
        {typeid(document::Text), [](const message_ptr& msg) { return std::make_shared<TextElement>(encoded(msg->get<document::Text>().text)); }},
        {typeid(document::Link), [this](const message_ptr& msg) { return this->write_link(msg->get<document::Link>()); }},
        {typeid(document::CloseLink), [](const message_ptr&) { return std::make_shared<TextElement>("</a>"); }},
        {typeid(document::Image), [this](const message_ptr& msg) { return this->write_image(msg->get<document::Image>()); }},
        {typeid(document::List), [this](const message_ptr& msg) { return this->write_list(msg->get<document::List>()); }},
        {typeid(document::CloseList), [](const message_ptr&) { return std::make_shared<TextElement>("</ul>"); }},
        {typeid(document::ListItem), [](const message_ptr&) { return std::make_shared<TextElement>("<li>"); }},
        {typeid(document::CloseListItem), [](const message_ptr&) { return std::make_shared<TextElement>("</li>"); }},
        {typeid(document::Header), [](const message_ptr&) { return std::make_shared<TextElement>("<header>"); }},
        {typeid(document::CloseHeader), [](const message_ptr&) { return std::make_shared<TextElement>("</header>"); }},
        {typeid(document::Footer), [](const message_ptr&) { return std::make_shared<TextElement>("<footer>"); }},
        {typeid(document::CloseFooter), [](const message_ptr&) { return std::make_shared<TextElement>("</footer>"); }},
        {typeid(document::Document), [this](const message_ptr& msg) {
            this->m_nested_docs_counter++;
            return this->m_nested_docs_counter == 1 ? this->write_open_header(msg->get<document::Document>()) : std::shared_ptr<TextElement>();
        }},
        {typeid(document::CloseDocument), [this](const message_ptr& msg) {
            throw_if(this->m_nested_docs_counter <= 0, errors::program_logic{});
            this->m_nested_docs_counter--;
            return this->m_nested_docs_counter == 0 ? this->write_footer() : std::shared_ptr<TextElement>();
        }},
        {typeid(document::Style), [this](const message_ptr& msg) { return this->write_style(msg->get<document::Style>()); }},
    }
  {}

  std::shared_ptr<TextElement>
  write_open_header(const document::Document& document)
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

  std::shared_ptr<TextElement> write_link(const document::Link& link)
  {
    HtmlAttrs attrs = styling_attributes(link);
    if (link.url)
      attrs.insert({"href", *link.url });
    return tag_with_attributes("a", attrs);
  }

  std::shared_ptr<TextElement> write_image(const document::Image& image)
  {
    HtmlAttrs attrs = styling_attributes(image.styling);
    if (image.alt)
        attrs.insert({"alt", *image.alt});
    std::string src_value;
    auto path_opt = image.source.path();
    if (path_opt)
        src_value = path_opt->string();
    else
    {
      auto image_mime_type = image.source.highest_confidence_mime_type();
      throw_if (!image_mime_type);
      std::string encoded_string = docwire::base64::encode(image.source.span());
      src_value = "data:" + image_mime_type->v + ";base64," + encoded_string;
    }
    attrs.insert({"src", src_value});
    return tag_with_attributes("img", attrs);
  }

  std::shared_ptr<TextElement> write_list(const document::List& list)
  {
    HtmlAttrs attrs = styling_attributes(list);
    std::string orig_style = attrs.count("style") ? attrs["style"] + "; " : "";
    std::string list_type = list.type;
    if (list_type != "decimal" && list_type != "disc" && list_type != "none")
      list_type = '"' + list_type + '"';
    attrs.insert_or_assign("style", orig_style + "list-style-type: " + list_type);
    return tag_with_attributes("ul", attrs);
  }

  std::shared_ptr<TextElement> write_style(const document::Style& style)
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

  void write_to(const message_ptr& msg, std::ostream &stream)
  {
    // Define a whitelist of message types that can appear in the <head> section.
    // Any other message type will cause the header to be closed and the body to be opened.
    bool is_header_content = msg->is<document::Style>() || msg->is<document::Document>();
    if (!is_header_content && m_header_is_open)
      write_close_header_open_body()->write_to(stream);

    auto it = m_handlers.find(std::type_index(msg->object_type()));
    std::shared_ptr<TextElement> text_element = (it != m_handlers.end())
                                                    ? it->second(msg)
                                                    : std::shared_ptr<TextElement>();
    if (text_element)
      text_element->write_to(stream);
  }
};

HtmlWriter::HtmlWriter()
  : with_pimpl<HtmlWriter>()
{}

void HtmlWriter::write_to(const message_ptr& msg, std::ostream &stream)
{
	impl().write_to(msg, stream);
}

} // namespace docwire
