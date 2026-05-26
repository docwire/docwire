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

#ifndef DOCWIRE_CONVERT_NUMERIC_H
#define DOCWIRE_CONVERT_NUMERIC_H

#include "convert_base.h"
#include <charconv>
#include "with_partial_match.h"
#include "core_export.h"

namespace docwire::convert {

// Concept for types supported by std::from_chars in C++20.
// This excludes bool, which is only supported from C++23.
/**
 * @brief Concept checking if a type is compatible with std::from_chars.
 */
template<typename T>
concept is_from_chars_compatible = (std::is_integral_v<T> && !std::is_same_v<T, bool>) || std::is_floating_point_v<T>;

namespace detail
{

template <typename T>
concept std_from_chars_available = requires(const char* first, const char* last, T& value) {
    { std::from_chars(first, last, value) } -> std::same_as<std::from_chars_result>;
};

DOCWIRE_CORE_EXPORT std::optional<float> from_chars_fallback(std::string_view sv, bool allow_partial, dest_type_tag<float>) noexcept;
DOCWIRE_CORE_EXPORT std::optional<double> from_chars_fallback(std::string_view sv, bool allow_partial, dest_type_tag<double>) noexcept;
DOCWIRE_CORE_EXPORT std::optional<long double> from_chars_fallback(std::string_view sv, bool allow_partial, dest_type_tag<long double>) noexcept;

} // namespace detail

/**
 * @brief Converts a string-like value to a numeric type using std::from_chars.
 * @tparam To The target numeric type.
 * @tparam From The source string type (must be convertible to string_view).
 */
template<is_from_chars_compatible To, std::convertible_to<std::string_view> From>
requires (noexcept(std::string_view(std::declval<const From&>())) && detail::std_from_chars_available<To>)
std::optional<To> convert_impl(const From& s, dest_type_tag<To>) noexcept
{
	To value{};
	const std::string_view sv(s);
	auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

    if (ec != std::errc() || ptr == sv.data()) // Check for conversion errors or if no characters were consumed.
        return std::nullopt;

    if constexpr (!std::is_same_v<From, with::partial_match>)
        if (ptr != sv.data() + sv.size())
            return std::nullopt;

    return value;
}

/**
 * @brief Fallback conversion for floating point types when std::from_chars is not available.
 */
template<std::floating_point To, std::convertible_to<std::string_view> From>
requires (noexcept(std::string_view(std::declval<const From&>())) && !detail::std_from_chars_available<To>)
std::optional<To> convert_impl(const From& s, dest_type_tag<To>) noexcept
{
    return detail::from_chars_fallback(std::string_view(s), std::is_same_v<From, with::partial_match>, dest_type_tag<To>{});
}

} // namespace docwire::convert

#endif // DOCWIRE_CONVERT_NUMERIC_H
