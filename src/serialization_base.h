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

#ifndef DOCWIRE_SERIALIZATION_BASE_H
#define DOCWIRE_SERIALIZATION_BASE_H

#include "concepts_container.h"
#include "concepts_misc.h"
#include "concepts_string.h"
#include "concepts_variant.h"
#include <cstdint>
#include <map>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include "type_name.h"

namespace docwire
{

/**
 * @brief Provides a generic, concept-based serialization framework.
 *
 * This namespace contains the tools to convert arbitrary C++ types into a structured,
 * serializable representation. The core of the framework is the `docwire::serialization::value`
 * type, a `std::variant` that can hold primitive types, arrays, or objects.
 *
 * The framework is designed to be non-intrusive. To add serialization support for a new type,
 * you specialize the `docwire::serialization::serializer` struct for that type. The framework
 * already provides specializations for many standard library types, primitives, and common
 * patterns like containers and pointers.
 *
 * This serialization mechanism is used extensively by the logging framework and error framework.
 * to capture the state of variables and objects for diagnostic purposes.
 */

namespace serialization
{

// Forward declarations for recursive variant types within the namespace
struct object;
struct array;

/**
 * @brief A variant type representing any serialized value.
 *
 * This is the cornerstone of the serialization framework, representing a value as one of
 * C++'s primitive types or a container of other `docwire::serialization::value`s.
 */
using value = std::variant<
    std::nullptr_t,
    bool,
    std::int64_t,
    std::uint64_t,
    double,
    std::string,
    array,
    object
>;

/// @brief Represents a serialized array (list of values).
struct array { std::vector<value> v; };

/// @brief Represents a serialized object (map of string keys to values).
struct object { std::map<std::string, value> v; };

/// @brief An enum to identify the kind of serializer specialization.
enum class serializer_kind
{
    value_alternative,
    arithmetic,
    string_like,
    container,
    dereferenceable
};

/// @brief Helper to decorate a serialized value with a typeid string.
inline value decorate_with_typeid(const value& base_val, const std::string& typeid_str)
{
    return object{{
        {"typeid", typeid_str},
        {"value", base_val}
    }};
}
/**
 * @brief Serializes a value of type T into a `docwire::serialization::value`.
 *
 * This function uses the `docwire::serialization::serializer` struct to perform the conversion.
 * @tparam T The type of the value to serialize.
 * @param value The value to serialize.
 * @return The serialized `docwire::serialization::value`.
 */
template <typename T> value full(const T& value);

/**
 * @brief Primary template for the serializer
 *
 * This struct is specialized for different types to provide a uniform way
 * to convert them into a `docwire::serialization::value`.
 * It defines two main operations: `full` for untyped serialization and `typed_summary` for typed serialization.
 */
template <typename T>
struct serializer;

template <typename T>
value full(const T& value) { return serializer<T>{}.full(value); }

template <typename T>
value typed_summary(const T& value)
{
    return serializer<T>{}.typed_summary(value);
}

/**
 * @brief A specific concept to check if a type `T` is one of the alternatives in `docwire::serialization::value`.
 */
template <typename T>
concept value_alternative = variant_alternative<T, value>;

/**
 * @brief Specialization for types that are direct alternatives in `docwire::serialization::value`.
 * This handles `bool`, `std::int64_t`, `std::uint64_t`, `double`, `std::string`, etc.
 */
template <value_alternative T>
struct serializer<T>
{
    static constexpr serializer_kind kind = serializer_kind::value_alternative;
	value full(const T& value) const { return value; }

    value typed_summary(const T& value) const {
        return decorate_with_typeid(this->full(value), type_name::pretty<T>());
    }
};

/**
 * @brief Specialization for arithmetic types (integers, floats).
 *
 * This converts various integer and floating-point types to the types
 * supported by the `docwire::serialization::value` variant.
 */
template <typename T> requires(std::is_arithmetic_v<T> && !value_alternative<T>)
struct serializer<T>
{
    static constexpr serializer_kind kind = serializer_kind::arithmetic;
    value full(const T& value) const
    {
        if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
            return static_cast<std::int64_t>(value);
        else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
            return static_cast<std::uint64_t>(value);
        else // is_floating_point
            return static_cast<double>(value);
    }

    value typed_summary(const T& value) const {
        return decorate_with_typeid(this->full(value), type_name::pretty<T>());
    }
};

/**
 * @brief Specialization for string-like types (e.g., const char*, std::string_view).
 */
template <typename T> requires string_like<T> && (!value_alternative<T>)
struct serializer<T>
{
    static constexpr serializer_kind kind = serializer_kind::string_like;
    value full(const T& val) const
    {
        if constexpr (std::is_pointer_v<std::decay_t<T>>) {
            if (val == nullptr) {
                return nullptr;
            }
        }
        return std::string(val);
    }
    value typed_summary(const T& val) const { return decorate_with_typeid(this->full(val), type_name::pretty<T>()); }
};

/**
 * @brief Specialization for strong type aliases.
 */
template <strong_type_alias T>
struct serializer<T>
{
    value full(const T& value) const { return serialization::full(value.v); }
    value typed_summary(const T& value) const { return decorate_with_typeid(full(value), type_name::pretty<T>()); }
};

/**
 * @brief Specialization for empty structs.
 */
template <empty T>
struct serializer<T>
{
    value full(const T& val) const { return object{}; }
    value typed_summary(const T& val) const { return decorate_with_typeid(this->full(val), type_name::pretty<T>()); }
};

/**
 * @brief Specialization for dereferenceable types (e.g., pointers, smart pointers).
 *
 * Serializes to `nullptr` if the pointer is null, otherwise serializes the dereferenced object.
 */
template <typename T>
requires (dereferenceable<T> && !container<T> && !string_like<T> && !value_alternative<T>)
struct serializer<T>
{
    static constexpr serializer_kind kind = serializer_kind::dereferenceable;

    value full(const T& dereferenceable) const
    {
        if (dereferenceable) {
            return object{{{"value", serialization::full(*dereferenceable)}}};
        }
        return nullptr;
    }

    value typed_summary(const T& dereferenceable) const {
        if (dereferenceable) {
            return object{{
                {"typeid", type_name::pretty<T>()},
                {"value", serialization::typed_summary(*dereferenceable)}
            }};
        }
        return decorate_with_typeid(nullptr, type_name::pretty<T>());
    }
};

/**
 * @brief Specialization for iterable types (e.g., std::vector, std::list).
 *
 * Serializes the container into a `docwire::serialization::array`.
 * It excludes std::string, which is iterable but should be treated as a single value.
 */
template <typename T> requires (container<T> && !string_like<T> && !value_alternative<T>)
struct serializer<T>
{
    static constexpr serializer_kind kind = serializer_kind::container;
    value full(const T& container) const
    {
        array arr;
        for (const auto& item : container) { arr.v.push_back(serialization::full(item)); }
        return arr;
    }

    value typed_summary(const T& container) const
    {
        array arr;
        for (const auto& item : container)
        {
            arr.v.push_back(serialization::typed_summary(item));
        }
        return decorate_with_typeid(arr, type_name::pretty<T>());
    }
};

/**
 * @brief Specialization for std::variant.
 *
 * Serializes the currently held value inside the variant. To preserve type
 * information about which variant member was active, it wraps the result
 * in an object.
 */
template<typename... Ts>
struct serializer<std::variant<Ts...>>
{
    value full(const std::variant<Ts...>& variant) const
	{
		return std::visit([](const auto& value) {
			return object{{{"value", serialization::full(value)}}};
		}, variant);
	}

    value typed_summary(const std::variant<Ts...>& variant) const { return decorate_with_typeid(this->full(variant), type_name::pretty<std::variant<Ts...>>()); }
};

} // namespace serialization

} // namespace docwire

#endif // DOCWIRE_SERIALIZATION_BASE_H
