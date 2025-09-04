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

#ifndef DOCWIRE_LOG_MESSAGES_H
#define DOCWIRE_LOG_MESSAGES_H

#include "attributes.h"
#include "document_elements.h"
#include "log.h"
#include "log_ctime.h" // IWYU pragma: keep
#include "log_data_source.h" // IWYU pragma: keep
#include "log_empty_struct.h" // IWYU pragma: keep
#include "log_exception.h" // IWYU pragma: keep
#include "log_variant.h" // IWYU pragma: keep 
#include "mail_elements.h"

namespace docwire
{

inline log_record_stream& operator<<(log_record_stream& log_stream, const attributes::Styling& styling)
{
	log_stream << docwire_log_streamable_obj(styling, styling.classes, styling.id, styling.style);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const attributes::Email& email)
{
	log_stream << docwire_log_streamable_obj(email, email.from, email.date, email.to, email.subject, email.reply_to, email.sender);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const attributes::Metadata& metadata)
{
	log_stream << docwire_log_streamable_obj(metadata, metadata.author, metadata.creation_date, metadata.last_modified_by, metadata.last_modification_date, metadata.page_count, metadata.word_count, metadata.email_attrs);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Paragraph& paragraph)
{
	log_stream << docwire_log_streamable_obj(paragraph, paragraph.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Section& section)
{
	log_stream << docwire_log_streamable_obj(section, section.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Span& span)
{
	log_stream << docwire_log_streamable_obj(span, span.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::BreakLine& break_line)
{
	log_stream << docwire_log_streamable_obj(break_line, break_line.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Bold& bold)
{
	log_stream << docwire_log_streamable_obj(bold, bold.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Italic& italic)
{
	log_stream << docwire_log_streamable_obj(italic, italic.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Underline& underline)
{
	log_stream << docwire_log_streamable_obj(underline, underline.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Table& table)
{
	log_stream << docwire_log_streamable_obj(table, table.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Caption& caption)
{
	log_stream << docwire_log_streamable_obj(caption, caption.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::TableRow& table_row)
{
	log_stream << docwire_log_streamable_obj(table_row, table_row.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::TableCell& table_cell)
{
	log_stream << docwire_log_streamable_obj(table_cell, table_cell.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Text& text)
{
	log_stream << docwire_log_streamable_obj(text, text.text);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Link& link)
{
	log_stream << docwire_log_streamable_obj(link, link.url, link.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Image& image)
{
	log_stream << docwire_log_streamable_obj(image, image.alt, image.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Style& style)
{
	log_stream << docwire_log_streamable_obj(style, style.css_text);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::List& list)
{
	log_stream << docwire_log_streamable_obj(list, list.type, list.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::ListItem& list_item)
{
	log_stream << docwire_log_streamable_obj(list_item, list_item.styling);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const mail::Mail& mail)
{
	log_stream << docwire_log_streamable_obj(mail, mail.subject, mail.date, mail.level);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const mail::Attachment& attachment)
{
	log_stream << docwire_log_streamable_obj(attachment, attachment.name, attachment.size);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const mail::Folder& folder)
{
	log_stream << docwire_log_streamable_obj(folder, folder.name, folder.level);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Comment& comment)
{
	log_stream << docwire_log_streamable_obj(comment, comment.author, comment.time, comment.comment);
	return log_stream;
}

inline log_record_stream& operator<<(log_record_stream& log_stream, const document::Document& document)
{
	return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_MESSAGES_H
