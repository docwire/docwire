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

#ifndef DOCWIRE_ATTRIBUTES_H
#define DOCWIRE_ATTRIBUTES_H

#include <chrono>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace docwire
{
/**
 * @brief Contains definitions for document attributes and metadata.
 */
namespace attributes
{

/**
 * @brief Represents CSS-like styling information for document elements.
 */
struct styling
{
  /// List of CSS classes.
  std::vector<std::string> classes;
  /// Unique identifier for the element.
  std::string id;
  /// Inline style string.
  std::string style;  
};

struct email
{
  /// The sender's email address.
  std::string from;
  /// The date and time the email was sent.
	std::chrono::sys_seconds date;
  /// The recipient's email address.
  std::optional<std::string> to;
  /// The subject of the email.
	std::optional<std::string> subject;
  /// The email address to reply to.
  std::optional<std::string> reply_to;
  /// The sender's name or address.
  std::optional<std::string> sender;
};

struct metadata
{
  /// The author of the document.
  std::optional<std::string> author;
  /// The creation date and time of the document.
  std::optional<std::chrono::sys_seconds> creation_date;
  /// The user who last modified the document.
  std::optional<std::string> last_modified_by;
  /// The last modification date and time of the document.
  std::optional<std::chrono::sys_seconds> last_modification_date;
  /// The number of pages in the document.
  std::optional<size_t> page_count;
  /// The number of words in the document.
  std::optional<size_t> word_count;
  /// Email-specific attributes if applicable.
  std::optional<email> email_attrs;
};

/**
 * @brief Concept for types that have styling information.
 */
template<class T>
concept WithStyling = requires(T tag) {
  {tag.styling} -> std::convertible_to<styling>;
};

/**
 * @brief Represents the geometric position and dimensions of an element.
 */
struct position
{
	std::optional<double> x;            ///< Optional x-coordinate of the bottom-left corner.
	std::optional<double> y;            ///< Optional y-coordinate of the bottom-left corner.
	std::optional<double> width;        ///< Optional width of the element.
	std::optional<double> height;       ///< Optional height of the element.
};

} // namespace attributes
} // namespace docwire

#endif // DOCWIRE_ATTRIBUTES_H