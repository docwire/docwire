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

#ifndef DOCWIRE_DOCUMENT_ELEMENTS_H
#define DOCWIRE_DOCUMENT_ELEMENTS_H

#include "attributes.h"
#include "core_export.h"
#include "data_source.h"
#include "message.h"
#include <functional>
#include <optional>
#include <string>

namespace docwire
{
namespace document
{

struct paragraph
{
  attributes::styling styling;
};

struct close_paragraph {};

struct section
{
  attributes::styling styling;
};

struct close_section {};

struct span
{
  attributes::styling styling;
};

struct close_span {};

struct break_line
{
  attributes::styling styling;
};

struct bold
{
  attributes::styling styling;
};

struct close_bold {};

struct italic
{
  attributes::styling styling;
};

struct close_italic {};

struct underline
{
  attributes::styling styling;
};

struct close_underline {};

struct table
{
  attributes::styling styling;
};

struct close_table {};

struct table_row
{
  attributes::styling styling;
};

struct close_table_row {};

struct table_cell
{
  attributes::styling styling;
};

struct close_table_cell {};

struct caption
{
  attributes::styling styling;
};

struct close_caption {};

struct text
{
  std::string text;
  attributes::position position; ///< Positional attributes.
  std::optional<double> font_size;    ///< Optional font size of the text.
};

struct link
{
  std::optional<std::string> url;
  attributes::styling styling;
};

struct close_link {};

struct image
{
  data_source source;
  std::optional<std::string> alt;     ///< Optional alternative text for the image.
  attributes::position position; ///< Positional attributes.
  attributes::styling styling;
  std::optional<message_sequence_streamer> structured_content_streamer;
};

struct style
{
  std::string css_text;
};

struct list
{
  std::string type = "decimal";
  attributes::styling styling;
};

struct close_list {};

struct list_item
{
  attributes::styling styling;
};

struct close_list_item {};

struct header {};
struct close_header {};

struct footer {};
struct close_footer {};

struct comment
{
  std::optional<std::string> author;
  std::optional<std::string> time;
  std::optional<std::string> comment;
};

struct page { };
struct close_page { };

struct document
{
  std::function<attributes::metadata()> metadata = []() { return attributes::metadata{}; };
};

struct close_document { };

} // namespace document
} // namespace docwire

#endif // DOCWIRE_DOCUMENT_ELEMENTS_H
