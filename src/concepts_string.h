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

#ifndef DOCWIRE_CONCEPTS_STRING_H
#define DOCWIRE_CONCEPTS_STRING_H

#include <concepts>
#include <string_view>
#include <type_traits>

namespace docwire
{

/**
 * @brief Concept for types that have a `string()` member method.
 */
template <typename T>
concept string_method_equipped = requires(const T& t) { { t.string() } -> std::convertible_to<std::string_view>; };

/**
 * @brief Concept for string-like types that can be converted to a string view.
 */
template<typename T>
concept string_like = std::is_convertible_v<T, std::string_view>;

} // namespace docwire

#endif // DOCWIRE_CONCEPTS_STRING_H
