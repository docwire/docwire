/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_DOCUMENT_ELEMENTS_H
#define DF0_DOCUMENT_ELEMENTS_H

#include "attributes.h"
#include "core_export.h"
#include "data_source.h"
#include "message.h"
#include <functional>
#include <optional>
#include <string>

namespace df0
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
} // namespace df0

#endif // DF0_DOCUMENT_ELEMENTS_H
