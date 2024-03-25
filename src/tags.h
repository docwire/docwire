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
#include "log.h"
#include "log_ctime.h"
#include "log_empty_struct.h"
#include "log_variant.h"
#include <fstream>
#include <optional>
#include <variant>
#include <vector>

namespace docwire
{

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace attributes
{

struct Styling
{
  std::vector<std::string> classes;
  std::string id;
  std::string style;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, classes, id, style);
  }
};

struct Email
{
  std::string from;
	tm date;
  std::optional<std::string> to;
	std::optional<std::string> subject;
  std::optional<std::string> reply_to;
  std::optional<std::string> sender;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, from, date, to, subject, reply_to, sender);
  }
};

template<class T>
concept WithStyling = requires(T tag) {
  {tag.styling} -> std::convertible_to<Styling>;
};

} // namespace attributes

namespace tag
{

struct PleaseWait {};

struct Paragraph
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseParagraph {};

struct Section
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseSection {};

struct Span
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseSpan {};

struct BreakLine
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct Bold
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseBold {};

struct Italic
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseItalic {};

struct Underline
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseUnderline {};

struct Table
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseTable {};

struct TableRow
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseTableRow {};

struct TableCell
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
};

struct CloseTableCell {};

struct Text
{
  std::string text;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, text);
  }
};

struct Link
{
  std::optional<std::string> url;
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, url, styling);
  }
};

struct CloseLink {};

struct Image
{
  std::string src;
  std::optional<std::string> alt;
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, src, alt, styling);
  }
};

struct Style
{
  std::string css_text;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, css_text);
  }
};

struct List
{
  std::string type = "decimal";
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, type, styling);
  }
};

struct CloseList {};

struct ListItem
{
  attributes::Styling styling;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, styling);
  }
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
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, subject, date, level);
  }
};

struct CloseMail {};

struct MailBody {};
struct CloseMailBody { };

struct Attachment
{
  std::optional<std::string> name;
  size_t size;
  std::optional<std::string> extension;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, name, size, extension);
  }
};

struct CloseAttachment { };

struct Folder
{
  std::optional<std::string> name;
  std::optional<int> level;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, name, level);
  }
};

struct CloseFolder { };

struct Metadata
{
  std::optional<std::string> author;
  std::optional<tm> creation_date;
  std::optional<std::string> last_modified_by;
  std::optional<tm> last_modification_date;
  std::optional<size_t> page_count;
  std::optional<size_t> word_count;
  std::optional<attributes::Email> email_attrs;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, author, creation_date, last_modified_by, last_modification_date, page_count, word_count, email_attrs);
  }
};

struct Comment
{
  std::optional<std::string> author;
  std::optional<std::string> time;
  std::optional<std::string> comment;
  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this, author, time, comment);
  }
};

struct Page { };
struct ClosePage { };

struct File
{
  std::variant<std::filesystem::path, std::shared_ptr<std::istream>> source;
	std::optional<std::string> name;

  std::shared_ptr<std::istream> access_stream() const
  {
    return std::visit(
      overloaded {
        [](const std::filesystem::path& source) {
          return std::shared_ptr<std::istream>(new std::ifstream(source, std::ios::binary));
        },
        [](std::shared_ptr<std::istream> source) {
          return source;
        }
      },
	  	source
    );
  };

  std::string access_name() const
  {
    return std::visit(
      overloaded {
        [](const std::filesystem::path& source) {
          return source.string();
        },
        [this](std::shared_ptr<std::istream> source) {
          return name.value_or("");
        }
      },
      source
    );
  }

  void log_to_record_stream(log_record_stream& s) const
  {
    s << docwire_log_streamable_obj(*this/*, source*/, name);
  }
};

struct CloseFile { };

struct Document { };
struct CloseDocument { };

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
  Metadata,
  Comment,
  Page,
  ClosePage,
  File,
  CloseFile,
  Document,
  CloseDocument
>;

}

using Tag = tag::Variant;

} // namespace docwire

#endif //DOCWIRE_TAGS_H
