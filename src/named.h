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
#ifndef DOCWIRE_NAMED_H
#define DOCWIRE_NAMED_H
#include <string_view>
#include <utility>
#include <tuple>
#include <type_traits>

/**
 * @brief Utilities for named parameters.
 */
namespace docwire::named
{
    /**
     * @brief A named value wrapper.
     */
    template <typename T>
    struct value
    {
        /// The name of the parameter.
        std::string_view name;
        /// The value of the parameter.
        T value;
    };

    template <size_t I, typename T>
    [[nodiscard]] constexpr decltype(auto) get(value<T>& v) noexcept
    {
        if constexpr (I == 0) return (v.name);
        else return (v.value);
    }

    template <size_t I, typename T>
    [[nodiscard]] constexpr decltype(auto) get(const value<T>& v) noexcept
    {
        if constexpr (I == 0) return (v.name);
        else return (v.value);
    }

    template <size_t I, typename T>
    [[nodiscard]] constexpr decltype(auto) get(value<T>&& v) noexcept
    {
        if constexpr (I == 0) return std::move(v.name);
        else return std::move(v.value);
    }

    /**
     * @brief A helper to create named values using assignment syntax.
     */
    struct variable
    {
        const std::string_view name;

        template <typename T>
        [[nodiscard]] constexpr auto operator=(T&& val) const noexcept(noexcept(value<std::decay_t<T>>{name, std::forward<T>(val)}))
        {
            return value<std::decay_t<T>>{name, std::forward<T>(val)};
        }
    };

    inline namespace literals
    {
        /**
         * @brief Literal operator to create named variables.
         * @example "my_var"_v = 42
         */
        [[nodiscard]] constexpr variable operator""_v(const char* str, size_t len) noexcept
        {
            return {std::string_view{str, len}};
        }
    }
}

namespace docwire
{
    // Bring the _v literal into the docwire namespace for convenience.
    using namespace docwire::named::literals;
}

namespace std
{
    /**
     * @brief Specialization of `std::tuple_size` for `docwire::named::value` to enable structured bindings.
     */
    template <typename T>
    struct tuple_size<docwire::named::value<T>> : std::integral_constant<size_t, 2> {};

    /**
     * @brief Specialization of `std::tuple_element` for `docwire::named::value` to enable structured bindings.
     */
    template <size_t I, typename T>
    struct tuple_element<I, docwire::named::value<T>>
    {
        static_assert(I < 2, "index out of range for docwire::named::value");
        using type = std::conditional_t<I == 0, std::string_view, T>;
    };
}

#endif // DOCWIRE_NAMED_H