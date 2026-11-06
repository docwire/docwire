/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ATTRIBUTES_H
#define DF0_ATTRIBUTES_H

#include <chrono>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace df0
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
} // namespace df0

#endif // DF0_ATTRIBUTES_H