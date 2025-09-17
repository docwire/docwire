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

struct DOCWIRE_CORE_EXPORT Paragraph
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseParagraph {};

struct DOCWIRE_CORE_EXPORT Section
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseSection {};

struct DOCWIRE_CORE_EXPORT Span
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseSpan {};

struct DOCWIRE_CORE_EXPORT BreakLine
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT Bold
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseBold {};

struct DOCWIRE_CORE_EXPORT Italic
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseItalic {};

struct DOCWIRE_CORE_EXPORT Underline
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseUnderline {};

struct DOCWIRE_CORE_EXPORT Table
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseTable {};

struct DOCWIRE_CORE_EXPORT TableRow
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseTableRow {};

struct DOCWIRE_CORE_EXPORT TableCell
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseTableCell {};

struct DOCWIRE_CORE_EXPORT Caption
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseCaption {};

struct DOCWIRE_CORE_EXPORT Text
{
  std::string text;
  attributes::Position position; ///< Positional attributes.
  std::optional<double> font_size;    ///< Optional font size of the text.
};

struct DOCWIRE_CORE_EXPORT Link
{
  std::optional<std::string> url;
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseLink {};

struct DOCWIRE_CORE_EXPORT Image
{
  data_source source;
  std::optional<std::string> alt;     ///< Optional alternative text for the image.
  attributes::Position position; ///< Positional attributes.
  attributes::Styling styling;
  std::optional<message_sequence_streamer> structured_content_streamer;
};

struct DOCWIRE_CORE_EXPORT Style
{
  std::string css_text;
};

struct DOCWIRE_CORE_EXPORT List
{
  std::string type = "decimal";
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseList {};

struct DOCWIRE_CORE_EXPORT ListItem
{
  attributes::Styling styling;
};

struct DOCWIRE_CORE_EXPORT CloseListItem {};

struct DOCWIRE_CORE_EXPORT Header {};
struct DOCWIRE_CORE_EXPORT CloseHeader {};

struct DOCWIRE_CORE_EXPORT Footer {};
struct DOCWIRE_CORE_EXPORT CloseFooter {};

struct DOCWIRE_CORE_EXPORT Comment
{
  std::optional<std::string> author;
  std::optional<std::string> time;
  std::optional<std::string> comment;
};

struct DOCWIRE_CORE_EXPORT Page { };
struct DOCWIRE_CORE_EXPORT ClosePage { };

struct DOCWIRE_CORE_EXPORT Document
{
  std::function<attributes::Metadata()> metadata = []() { return attributes::Metadata{}; };
};

struct DOCWIRE_CORE_EXPORT CloseDocument { };

} // namespace document
} // namespace docwire

#endif // DOCWIRE_DOCUMENT_ELEMENTS_H
