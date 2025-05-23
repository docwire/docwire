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


#ifndef DOCWIRE_TAGS_H
#define DOCWIRE_TAGS_H

#include <ctime>
#include "data_source.h"
#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace docwire
{

namespace attributes
{

struct Styling
{
  std::vector<std::string> classes;
  std::string id;
  std::string style;  
};

struct Email
{
  std::string from;
	tm date;
  std::optional<std::string> to;
	std::optional<std::string> subject;
  std::optional<std::string> reply_to;
  std::optional<std::string> sender;
};

struct Metadata
{
  std::optional<std::string> author;
  std::optional<tm> creation_date;
  std::optional<std::string> last_modified_by;
  std::optional<tm> last_modification_date;
  std::optional<size_t> page_count;
  std::optional<size_t> word_count;
  std::optional<attributes::Email> email_attrs;
};

template<class T>
concept WithStyling = requires(T tag) {
  {tag.styling} -> std::convertible_to<Styling>;
};

} // namespace attributes

enum class continuation { proceed, skip, stop };
struct emission_callbacks;
using tag_sequence_streamer = std::function<continuation(const emission_callbacks&)>;

namespace tag
{

struct PleaseWait {};

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
  std::optional<std::string> alt;
  attributes::Styling styling;
  std::optional<tag_sequence_streamer> structured_content_streamer;
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

struct Mail
{
  std::optional<std::string> subject;
  std::optional<uint32_t> date;
  std::optional<int> level;
};

struct CloseMail {};

struct MailBody {};
struct CloseMailBody { };

struct Attachment
{
  std::optional<std::string> name;
  size_t size;
  std::optional<file_extension> extension;
};

struct CloseAttachment { };

struct Folder
{
  std::optional<std::string> name;
  std::optional<int> level;
};

struct CloseFolder { };

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

struct start_processing {};

using Variant = std::variant<
  PleaseWait,
  Paragraph,
  CloseParagraph,
  Section,
  CloseSection,
  Span,
  CloseSpan,
  BreakLine,
  Bold,
  CloseBold,
  Italic,
  CloseItalic,
  Underline,
  CloseUnderline,
  Table,
  CloseTable,
  Caption,
  CloseCaption,
  TableRow,
  CloseTableRow,
  TableCell,
  CloseTableCell,
  Text,
  Link,
  CloseLink,
  Image,
  Style,
  List,
  CloseList,
  ListItem,
  CloseListItem,
  Header,
  CloseHeader,
  Footer,
  CloseFooter,
  Mail,
  CloseMail,
  MailBody,
  CloseMailBody,
  Attachment,
  CloseAttachment,
  Folder,
  CloseFolder,
  Comment,
  Page,
  ClosePage,
  data_source,
  Document,
  CloseDocument,
  start_processing,
  std::exception_ptr
>;

}

using Tag = tag::Variant;

struct emission_callbacks
{
  std::function<continuation(Tag&&)> further;
  std::function<continuation(Tag&&)> back;
  continuation operator()(Tag&& tag) const { return further(std::move(tag)); }
};

} // namespace docwire

#endif //DOCWIRE_TAGS_H
