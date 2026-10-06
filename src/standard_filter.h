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

#ifndef DOCWIRE_STANDARD_FILTER_H
#define DOCWIRE_STANDARD_FILTER_H

#include "pipeline/element_base.hpp"
#include "core_export.h"
#include "file_extension.h"
#include "mail_elements.h"
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace docwire
{

/**
 * @brief A chain element that skips messages not belonging to one of the
 *        specified mail folders.
 */
class filter_by_folder_name : public pipeline::transformer_element<filter_by_folder_name>
{
public:
  explicit filter_by_folder_name(std::vector<std::string> names)
    : m_names{std::move(names)}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message) const
  {
    if (!msg->is<mail::folder>())
      return emit_message(std::move(msg));
    auto folder_name = msg->get<mail::folder>().name;
    if (folder_name)
    {
      if (!std::any_of(m_names.begin(), m_names.end(),
              [&folder_name](const std::string& name) { return (*folder_name) == name; }))
        return continuation::skip;
    }
    return emit_message(std::move(msg));
  }

private:
  std::vector<std::string> m_names;
};

/**
 * @brief A chain element that skips attachments not matching one of the
 *        specified file extensions.
 */
class filter_by_attachment_type : public pipeline::transformer_element<filter_by_attachment_type>
{
public:
  explicit filter_by_attachment_type(std::vector<file_extension> types)
    : m_types{std::move(types)}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message) const
  {
    if (!msg->is<mail::attachment>())
      return emit_message(std::move(msg));
    auto attachment_type = msg->get<mail::attachment>().extension;
    if (attachment_type)
    {
      if (!std::any_of(m_types.begin(), m_types.end(),
              [&attachment_type](const file_extension& type) { return (*attachment_type) == type; }))
        return continuation::skip;
    }
    return emit_message(std::move(msg));
  }

private:
  std::vector<file_extension> m_types;
};

/**
 * @brief A chain element that skips mail messages created before the given
 *        minimum time.
 */
class filter_by_mail_min_creation_time : public pipeline::transformer_element<filter_by_mail_min_creation_time>
{
public:
  explicit filter_by_mail_min_creation_time(unsigned int min_time)
    : m_min_time{min_time}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message) const
  {
    if (!msg->is<mail::mail>())
      return emit_message(std::move(msg));
    auto mail_creation_time = msg->get<mail::mail>().date;
    if (mail_creation_time)
    {
      if (*mail_creation_time < m_min_time)
        return continuation::skip;
    }
    return emit_message(std::move(msg));
  }

private:
  unsigned int m_min_time;
};

/**
 * @brief A chain element that skips mail messages created after the given
 *        maximum time.
 */
class filter_by_mail_max_creation_time : public pipeline::transformer_element<filter_by_mail_max_creation_time>
{
public:
  explicit filter_by_mail_max_creation_time(unsigned int max_time)
    : m_max_time{max_time}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message) const
  {
    if (!msg->is<mail::mail>())
      return emit_message(std::move(msg));
    auto mail_creation_time = msg->get<mail::mail>().date;
    if (mail_creation_time)
    {
      if (*mail_creation_time > m_max_time)
        return continuation::skip;
    }
    return emit_message(std::move(msg));
  }

private:
  unsigned int m_max_time;
};

/**
 * @brief A chain element that stops the pipeline after a given number of
 *        messages.
 */
class filter_by_max_node_number : public pipeline::transformer_element<filter_by_max_node_number>
{
public:
  explicit filter_by_max_node_number(unsigned int max_nodes)
    : m_max_nodes{max_nodes}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message)
  {
    if (m_node_no++ == m_max_nodes)
      return continuation::stop;
    return emit_message(std::move(msg));
  }

private:
  unsigned int m_max_nodes;
  unsigned int m_node_no{0};
};

/**
 * @brief Sets of standard filters to use in parsers.
 * example of use:
 * @code
 * std::filesystem::path{"test.pst"} | content_type::by_file_extension::detector{} | pst_parser{} |
 *  standard_filter::filterByFolderName({"Inbox", "Sent"}) |
 *  standard_filter::filterByAttachmentType({"jpg", "png"}) |
 *  plain_text_exporter{};
 * @endcode
 */
class standard_filter
{
public:
  static filter_by_folder_name filterByFolderName(std::vector<std::string> names)
  {
    return filter_by_folder_name{std::move(names)};
  }

  static filter_by_attachment_type filterByAttachmentType(std::vector<file_extension> types)
  {
    return filter_by_attachment_type{std::move(types)};
  }

  static filter_by_mail_min_creation_time filterByMailMinCreationTime(unsigned int min_time)
  {
    return filter_by_mail_min_creation_time{min_time};
  }

  static filter_by_mail_max_creation_time filterByMailMaxCreationTime(unsigned int max_time)
  {
    return filter_by_mail_max_creation_time{max_time};
  }

  static filter_by_max_node_number filterByMaxNodeNumber(unsigned int max_nodes)
  {
    return filter_by_max_node_number{max_nodes};
  }
};

} // namespace docwire

#endif //DOCWIRE_STANDARD_FILTER_H
