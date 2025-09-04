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

#ifndef DOCWIRE_DOCUMENT_ELEMENTS_H
#define DOCWIRE_DOCUMENT_ELEMENTS_H

#include "attributes.h"
#include "data_source.h"
#include "message.h"
#include <functional>
#include <optional>
#include <string>

namespace docwire
{
namespace document
{

struct Paragraph
{
  attributes::Styling styling;
};

struct CloseParagraph {};

struct Section
{
  attributes::Styling styling;
};

struct CloseSection {};

struct Span
{
  attributes::Styling styling;
};

struct CloseSpan {};

struct BreakLine
{
  attributes::Styling styling;
};

struct Bold
{
  attributes::Styling styling;
};

struct CloseBold {};

struct Italic
{
  attributes::Styling styling;
};

struct CloseItalic {};

struct Underline
{
  attributes::Styling styling;
};

struct CloseUnderline {};

struct Table
{
  attributes::Styling styling;
};

struct CloseTable {};

struct TableRow
{
  attributes::Styling styling;
};

struct CloseTableRow {};

struct TableCell
{
  attributes::Styling styling;
};

struct CloseTableCell {};

struct Caption
{
  attributes::Styling styling;
};

struct CloseCaption {};

struct Text
{
  std::string text;
  attributes::Position position; ///< Positional attributes.
  std::optional<double> font_size;    ///< Optional font size of the text.
};

struct Link
{
  std::optional<std::string> url;
  attributes::Styling styling;
};

struct CloseLink {};

struct Image
{
  data_source source;
  std::optional<std::string> alt;     ///< Optional alternative text for the image.
  attributes::Position position; ///< Positional attributes.
  attributes::Styling styling;
  std::optional<message_sequence_streamer> structured_content_streamer;
};

struct Style
{
  std::string css_text;
};

struct List
{
  std::string type = "decimal";
  attributes::Styling styling;
};

struct CloseList {};

struct ListItem
{
  attributes::Styling styling;
};

struct CloseListItem {};

struct Header {};
struct CloseHeader {};

struct Footer {};
struct CloseFooter {};

struct Comment
{
  std::optional<std::string> author;
  std::optional<std::string> time;
  std::optional<std::string> comment;
};

struct Page { };
struct ClosePage { };

struct Document
{
  std::function<attributes::Metadata()> metadata = []() { return attributes::Metadata{}; };
};

struct CloseDocument { };

} // namespace document
} // namespace docwire

#endif // DOCWIRE_DOCUMENT_ELEMENTS_H
