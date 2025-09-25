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

#ifndef DOCWIRE_ATTRIBUTES_H
#define DOCWIRE_ATTRIBUTES_H

#include <concepts>
#include <cstddef>
#include <ctime>
#include <optional>
#include <string>
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
  std::optional<Email> email_attrs;
};

template<class T>
concept WithStyling = requires(T tag) {
  {tag.styling} -> std::convertible_to<Styling>;
};

/**
 * @brief Represents the geometric position and dimensions of an element.
 */
struct Position
{
	std::optional<double> x;            ///< Optional x-coordinate of the bottom-left corner.
	std::optional<double> y;            ///< Optional y-coordinate of the bottom-left corner.
	std::optional<double> width;        ///< Optional width of the element.
	std::optional<double> height;       ///< Optional height of the element.
};

} // namespace attributes
} // namespace docwire

#endif // DOCWIRE_ATTRIBUTES_H