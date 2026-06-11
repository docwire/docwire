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

#ifndef DOCWIRE_STRINGIFICATION_H
#define DOCWIRE_STRINGIFICATION_H

#include "concepts_misc.h"
#include "concepts_stream.h"
#include "concepts_string.h"
#include "diagnostic_message.h"
#include "named.h"
#include "serialization_base.h"
#include <string>
#include <sstream>
#include <type_traits>

namespace docwire
{

/**
 * @brief Primary template for the stringifier.
 * @tparam T The type to be stringified.
 */
template <typename T>
struct stringifier;

template <typename T>
std::string stringify(const T& value)
{
	return stringifier<T>()(value);
}

/**
 * @brief Specialization for types that are streamable to `std::ostream`.
 */
template <streamable T>
requires (!string_method_equipped<T>) // Avoid conflict if T also has a string() method
struct stringifier<T>
{
	std::string operator()(const T& value) const
	{
		std::ostringstream s;
		s << value;
		return s.str();
	}
};

/**
 * @brief Specialization for types with a `string()` method.
 */
template <string_method_equipped T>
struct stringifier<T>
{
	std::string operator()(const T& value) const { return std::string(value.string()); }
};

/**
 * @brief Default stringifier for types not covered by more specific specializations.
 *
 * This fallback uses the generic `docwire::serialization` mechanism:
 * 1. Serializes the object to a `docwire::serialization::value`.
 * 2. Converts that `docwire::serialization::value` to a human-readable string.
 *
 * This ensures that any type that can be serialized can also be stringified in a default way,
 * providing a consistent fallback for complex types.
 * @tparam T The type to be stringified.
 */
template <typename T>
requires (!string_method_equipped<T> && !streamable<T> && !strong_type_alias<T>)
struct stringifier<T>
{
	std::string operator()(const T& value) const
	{
		return stringify(serialization::full(value));
	}
};

template<>
struct stringifier<const char*>
{
	std::string operator()(const char* value) const { return value; }
};

/**
 * @brief Specialization for `std::exception_ptr`.
 */
template <>
struct stringifier<std::exception_ptr>
{
	std::string operator()(const std::exception_ptr& eptr) const { return errors::diagnostic_message(eptr); }
};

// Specialization for std::pair, providing a custom string representation
template <typename T1, typename T2>
struct stringifier<std::pair<T1, T2>>
{
	std::string operator()(const std::pair<T1, T2>& pair) const
	{
		return stringify(pair.first) + ": " + stringify(pair.second);
	}
};

/**
 * @brief Specialization for `docwire::named::value`, providing a "name: value" string representation.
 */
template <typename T>
struct stringifier<named::value<T>>
{
    std::string operator()(const named::value<T>& nv) const
    {
        return stringify(nv.name) + ": " + stringify(nv.value);
    }
};

template <strong_type_alias T>
requires (
	!std::is_same_v<T, serialization::object> &&
	!std::is_same_v<T, serialization::array>)
struct stringifier<T>
{
	std::string operator()(const T& value) const { return stringify(value.v); }
};

template<>
struct stringifier<std::string>
{
    std::string operator()(const std::string& value) const { return value; }
};

template<>
struct stringifier<serialization::object>
{
	std::string operator()(const serialization::object& obj) const
	{
		return stringify(serialization::value{obj});
	}
};

template<>
struct stringifier<serialization::value>
{
	std::string operator()(const serialization::value& s_val) const
	{
		return std::visit(
			[](auto&& arg) -> std::string {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, serialization::object>)
				{
					std::string result = "{";
					bool first = true;
					for (const auto& [key, val] : arg.v)
					{
						if (!first) result += ", ";
						result += key + ": " + stringify(val);
						first = false;
					}
					result += "}";
					return result;
				}
				else if constexpr (std::is_same_v<T, serialization::array>)
				{
					std::string result = "[";
					bool first = true;
					for (const auto& val : arg.v)
					{
						if (!first) result += ", ";
						result += stringify(val);
						first = false;
					}
					result += "]";
					return result;
				}
				else if constexpr (std::is_same_v<T, std::nullptr_t>)
				{
					return "nullptr";
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					return arg ? "true" : "false";
				}
				else
				{
					return stringify(arg);
				}
			},
			s_val);
	}
};

} // namespace docwire

#endif
