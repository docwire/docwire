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

#include "html_writer.h"

#include "convert_chrono.h"  // IWYU pragma: keep
#include <memory>
#include <map>
#include "base64.h"
#include <boost/container/flat_map.hpp>
#include "error_tags.h"
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

HtmlAttrs styling_attributes(const attributes::styling& styling)
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

std::shared_ptr<text_element> tag_with_attributes(const std::string& tag_name, const HtmlAttrs& attributes)
{
  return std::make_shared<text_element>("<" + tag_name + to_string(attributes) + ">");
}

} // anonymous namespace

template<>
struct pimpl_impl<html_writer> : pimpl_impl_base
{
  bool m_header_is_open { false };
  int m_nested_docs_counter { 0 };
  using handler_func = std::function<std::shared_ptr<text_element>(const message_ptr&)>;
  const boost::container::flat_map<std::type_index, handler_func> m_handlers;

  pimpl_impl()
    : m_handlers{
        {typeid(document::paragraph), [](const message_ptr& msg) { return tag_with_attributes("p", styling_attributes(msg->get<document::paragraph>())); }},
        {typeid(document::close_paragraph), [](const message_ptr&) { return std::make_shared<text_element>("</p>"); }},
        {typeid(document::section), [](const message_ptr& msg) { return tag_with_attributes("div", styling_attributes(msg->get<document::section>())); }},
        {typeid(document::close_section), [](const message_ptr&) { return std::make_shared<text_element>("</div>"); }},
        {typeid(document::span), [](const message_ptr& msg) { return tag_with_attributes("span", styling_attributes(msg->get<document::span>())); }},
        {typeid(document::close_span), [](const message_ptr&) { return std::make_shared<text_element>("</span>"); }},
        {typeid(document::bold), [](const message_ptr& msg) { return tag_with_attributes("b", styling_attributes(msg->get<document::bold>())); }},
        {typeid(document::close_bold), [](const message_ptr&) { return std::make_shared<text_element>("</b>"); }},
        {typeid(document::italic), [](const message_ptr& msg) { return tag_with_attributes("i", styling_attributes(msg->get<document::italic>())); }},
        {typeid(document::close_italic), [](const message_ptr&) { return std::make_shared<text_element>("</i>"); }},
        {typeid(document::underline), [](const message_ptr& msg) { return tag_with_attributes("u", styling_attributes(msg->get<document::underline>())); }},
        {typeid(document::close_underline), [](const message_ptr&) { return std::make_shared<text_element>("</u>"); }},
        {typeid(document::table), [](const message_ptr& msg) { return tag_with_attributes("table", styling_attributes(msg->get<document::table>())); }},
        {typeid(document::close_table), [](const message_ptr&) { return std::make_shared<text_element>("</table>"); }},
        {typeid(document::table_row), [](const message_ptr& msg) { return tag_with_attributes("tr", styling_attributes(msg->get<document::table_row>())); }},
        {typeid(document::close_table_row), [](const message_ptr&) { return std::make_shared<text_element>("</tr>"); }},
        {typeid(document::table_cell), [](const message_ptr& msg) { return tag_with_attributes("td", styling_attributes(msg->get<document::table_cell>())); }},
        {typeid(document::close_table_cell), [](const message_ptr&) { return std::make_shared<text_element>("</td>"); }},
        {typeid(document::caption), [](const message_ptr& msg) { return tag_with_attributes("caption", styling_attributes(msg->get<document::caption>())); }},
        {typeid(document::close_caption), [](const message_ptr&) { return std::make_shared<text_element>("</caption>"); }},
        {typeid(document::break_line), [](const message_ptr& msg) { return tag_with_attributes("br", styling_attributes(msg->get<document::break_line>())); }},
        {typeid(document::text), [](const message_ptr& msg) { return std::make_shared<text_element>(encoded(msg->get<document::text>().text)); }},
        {typeid(document::link), [this](const message_ptr& msg) { return this->write_link(msg->get<document::link>()); }},
        {typeid(document::close_link), [](const message_ptr&) { return std::make_shared<text_element>("</a>"); }},
        {typeid(document::image), [this](const message_ptr& msg) { return this->write_image(msg->get<document::image>()); }},
        {typeid(document::list), [this](const message_ptr& msg) { return this->write_list(msg->get<document::list>()); }},
        {typeid(document::close_list), [](const message_ptr&) { return std::make_shared<text_element>("</ul>"); }},
        {typeid(document::list_item), [](const message_ptr&) { return std::make_shared<text_element>("<li>"); }},
        {typeid(document::close_list_item), [](const message_ptr&) { return std::make_shared<text_element>("</li>"); }},
        {typeid(document::header), [](const message_ptr&) { return std::make_shared<text_element>("<header>"); }},
        {typeid(document::close_header), [](const message_ptr&) { return std::make_shared<text_element>("</header>"); }},
        {typeid(document::footer), [](const message_ptr&) { return std::make_shared<text_element>("<footer>"); }},
        {typeid(document::close_footer), [](const message_ptr&) { return std::make_shared<text_element>("</footer>"); }},
        {typeid(document::document), [this](const message_ptr& msg) {
            this->m_nested_docs_counter++;
            return this->m_nested_docs_counter == 1 ? this->write_open_header(msg->get<document::document>()) : std::shared_ptr<text_element>();
        }},
        {typeid(document::close_document), [this](const message_ptr& msg) {
            throw_if(this->m_nested_docs_counter <= 0, errors::program_logic{});
            this->m_nested_docs_counter--;
            return this->m_nested_docs_counter == 0 ? this->write_footer() : std::shared_ptr<text_element>();
        }},
        {typeid(document::style), [this](const message_ptr& msg) { return this->write_style(msg->get<document::style>()); }},
    }
  {}

  std::shared_ptr<text_element>
  write_open_header(const document::document& document)
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
    return std::make_shared<text_element>(header);
  }

  std::shared_ptr<text_element> write_close_header_open_body()
  {
    m_header_is_open = false;
    return std::make_shared<text_element>("</head>\n<body>\n");
  }

  std::shared_ptr<text_element>
  write_footer()
  {
    std::string footer = {"</body>\n"
           "</html>\n"};
    return std::make_shared<text_element>(footer);
  }

  std::shared_ptr<text_element> write_link(const document::link& link)
  {
    HtmlAttrs attrs = styling_attributes(link);
    if (link.url)
      attrs.insert({"href", *link.url });
    return tag_with_attributes("a", attrs);
  }

  std::shared_ptr<text_element> write_image(const document::image& image)
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

  std::shared_ptr<text_element> write_list(const document::list& list)
  {
    HtmlAttrs attrs = styling_attributes(list);
    std::string orig_style = attrs.count("style") ? attrs["style"] + "; " : "";
    std::string list_type = list.type;
    if (list_type != "decimal" && list_type != "disc" && list_type != "none")
      list_type = '"' + list_type + '"';
    attrs.insert_or_assign("style", orig_style + "list-style-type: " + list_type);
    return tag_with_attributes("ul", attrs);
  }

  std::shared_ptr<text_element> write_style(const document::style& style)
  {
    return std::make_shared<text_element>(
        "<style type=\"text/css\">" + style.css_text + "</style>\n"/* : ""*/);
  }

  std::shared_ptr<text_element> write_metadata(const attributes::metadata& metadata)
  {
    std::string meta;
    if (metadata.author)
      meta += "<meta name=\"author\" content=\"" + encoded(*metadata.author) + "\">\n";
    if (metadata.creation_date)
      meta += "<meta name=\"creation-date\" content=\"" + convert::to<std::string>(*metadata.creation_date) + "\">\n";
    if (metadata.last_modified_by)
      meta += "<meta name=\"last-modified-by\" content=\"" + encoded(*metadata.last_modified_by) + "\">\n";
    if (metadata.last_modification_date)
      meta += "<meta name=\"last-modification-date\" content=\"" + convert::to<std::string>(*metadata.last_modification_date) + "\">\n";
    if (metadata.email_attrs)
    {
      meta += "<meta name=\"from\" content=\"" + encoded(metadata.email_attrs->from) + "\">\n";
      meta += "<meta name=\"date\" content=\"" + convert::to<std::string>(metadata.email_attrs->date) + "\">\n";
      meta += "<meta name=\"to\" content=\"" + encoded(*metadata.email_attrs->to) + "\">\n";
      meta += "<meta name=\"subject\" content=\"" + encoded(*metadata.email_attrs->subject) + "\">\n";
      meta += "<meta name=\"reply-to\" content=\"" + encoded(*metadata.email_attrs->reply_to) + "\">\n";
      meta += "<meta name=\"sender\" content=\"" + encoded(*metadata.email_attrs->sender) + "\">\n";
    }
    return std::make_shared<text_element>(meta);
  }

  void write_to(const message_ptr& msg, std::ostream &stream)
  {
    // Define a whitelist of message types that can appear in the <head> section.
    // Any other message type will cause the header to be closed and the body to be opened.
    bool is_header_content = msg->is<document::style>() || msg->is<document::document>();
    if (!is_header_content && m_header_is_open)
      write_close_header_open_body()->write_to(stream);

    auto it = m_handlers.find(std::type_index(msg->object_type()));
    std::shared_ptr<text_element> text_element = (it != m_handlers.end())
                                                    ? it->second(msg)
                                                    : std::shared_ptr<docwire::text_element>();
    if (text_element)
      text_element->write_to(stream);
  }
};

html_writer::html_writer()
  : with_pimpl<html_writer>()
{}

void html_writer::write_to(const message_ptr& msg, std::ostream &stream)
{
	impl().write_to(msg, stream);
}

} // namespace docwire
