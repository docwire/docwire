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

#ifndef DOCWIRE_CONVERT_BASE_H
#define DOCWIRE_CONVERT_BASE_H

#include <optional>
#include <tuple>
#include "error_tags.h"
#include "make_error.h"
#include "source_location.h"
#include "type_name.h"

/**
 * @brief Namespace for type conversion utilities.
 */
namespace docwire::convert
{

/**
 * @brief A tag type used for dispatching `convert_impl` overloads based on the destination type.
 */
template<typename T>
struct dest_type_tag {};

template <typename To, typename From>
concept conversion_implementation_exists = requires(const From& from) {
    { convert_impl(from, convert::dest_type_tag<To>{}) } noexcept -> std::same_as<std::optional<To>>;
};

namespace detail
{

struct convert_cpo
{
    template<typename To, typename From>
    requires conversion_implementation_exists<To, From>
    constexpr std::optional<To> operator()(const From& from) const noexcept
    {
        return convert_impl(from, dest_type_tag<To>{});
    }
};

inline constexpr convert_cpo convert_cpo;

} // namespace detail

/**
 * @brief Tries to convert a value of type From to type To.
 * @tparam To The target type.
 * @tparam From The source type.
 * @param from The value to convert.
 * @return An optional containing the converted value if successful, or std::nullopt otherwise.
 */
template<typename To, typename From>
requires conversion_implementation_exists<To, From>
constexpr std::optional<To> try_to(const From& from) noexcept
{
	return detail::convert_cpo.template operator()<To, From>(from);
}

/**
 * @brief Converts a value of type From to type To.
 * @tparam To The target type.
 * @tparam From The source type.
 * @param from The value to convert.
 * @throws docwire::error with docwire::errors::uninterpretable_data tag attached if the conversion fails.
 */
template<typename To, typename From>
requires conversion_implementation_exists<To, From>
To to(const From& from)
{
	auto result = try_to<To>(from);
	if (!result.has_value())
	{
		auto context = std::make_tuple("Failed to convert value", "from_type"_v = type_name::pretty<From>(), "to_type"_v = type_name::pretty<To>(), errors::uninterpretable_data{});
		throw errors::make_error_from_tuple(source_location::current(), std::move(context));
	}
	return *result;
}

} // namespace docwire::convert

#endif // DOCWIRE_CONVERT_BASE_H
