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

#ifndef DOCWIRE_STANDARD_FILTER_H
#define DOCWIRE_STANDARD_FILTER_H

#include "core_export.h"
#include "transformer_func.h"

namespace docwire
{
/**
 * @brief Sets of standard filters to use in parsers.
 * example of use:
 * @code
 * std::filesystem::path{"test.pst"} | content_type::by_file_extension::detector{} | PSTParser{} |
 *  StandardFilter::filterByFolderName({"Inbox", "Sent"}) |
 *  StandardFilter::filterByAttachmentType({"jpg", "png"}) |
 *  PlainTextExporter{};
 * @endcode
 */
class DOCWIRE_CORE_EXPORT StandardFilter
{
public:
  /**
   * @brief Filters folders by name. Keeps only folders with names that exist in the given list.
   * @param names list of names to keep
   */
  static NewNodeCallback filterByFolderName(const std::vector<std::string> &names);

  /**
   * @brief Filters attachments by type. Keeps only attachments with type that exist in the given list.
   * @param types list of types to keep
   */
  static NewNodeCallback filterByAttachmentType(const std::vector<file_extension>& types);

  /**
   * @brief Filters mail by creation date. Keeps only mails that are created after the given date.
   * @param min_time minimum time to keep
   */
  static NewNodeCallback filterByMailMinCreationTime(unsigned int min_time);

  /**
   * @brief Filters mail by creation date. Keeps only mails that are created before the given date.
   * @param max_time maximum time to keep
   */
  static NewNodeCallback filterByMailMaxCreationTime(unsigned int max_time);

  /**
   * @brief
   * @param max_nodes
   */
  static NewNodeCallback filterByMaxNodeNumber(unsigned int max_nodes);
};
} // namespace docwire

#endif //DOCWIRE_STANDARD_FILTER_H
