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

struct DOCWIRE_CORE_EXPORT paragraph
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_paragraph {};

struct DOCWIRE_CORE_EXPORT section
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_section {};

struct DOCWIRE_CORE_EXPORT span
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_span {};

struct DOCWIRE_CORE_EXPORT break_line
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT bold
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_bold {};

struct DOCWIRE_CORE_EXPORT italic
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_italic {};

struct DOCWIRE_CORE_EXPORT underline
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_underline {};

struct DOCWIRE_CORE_EXPORT table
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_table {};

struct DOCWIRE_CORE_EXPORT table_row
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_table_row {};

struct DOCWIRE_CORE_EXPORT table_cell
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_table_cell {};

struct DOCWIRE_CORE_EXPORT caption
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_caption {};

struct DOCWIRE_CORE_EXPORT text
{
  std::string text;
  attributes::position position; ///< Positional attributes.
  std::optional<double> font_size;    ///< Optional font size of the text.
};

struct DOCWIRE_CORE_EXPORT link
{
  std::optional<std::string> url;
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_link {};

struct DOCWIRE_CORE_EXPORT image
{
  data_source source;
  std::optional<std::string> alt;     ///< Optional alternative text for the image.
  attributes::position position; ///< Positional attributes.
  attributes::styling styling;
  std::optional<message_sequence_streamer> structured_content_streamer;
};

struct DOCWIRE_CORE_EXPORT style
{
  std::string css_text;
};

struct DOCWIRE_CORE_EXPORT list
{
  std::string type = "decimal";
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_list {};

struct DOCWIRE_CORE_EXPORT list_item
{
  attributes::styling styling;
};

struct DOCWIRE_CORE_EXPORT close_list_item {};

struct DOCWIRE_CORE_EXPORT header {};
struct DOCWIRE_CORE_EXPORT close_header {};

struct DOCWIRE_CORE_EXPORT footer {};
struct DOCWIRE_CORE_EXPORT close_footer {};

struct DOCWIRE_CORE_EXPORT comment
{
  std::optional<std::string> author;
  std::optional<std::string> time;
  std::optional<std::string> comment;
};

struct DOCWIRE_CORE_EXPORT page { };
struct DOCWIRE_CORE_EXPORT close_page { };

struct DOCWIRE_CORE_EXPORT document
{
  std::function<attributes::metadata()> metadata = []() { return attributes::metadata{}; };
};

struct DOCWIRE_CORE_EXPORT close_document { };

} // namespace document
} // namespace docwire

#endif // DOCWIRE_DOCUMENT_ELEMENTS_H
