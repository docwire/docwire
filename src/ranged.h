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

#ifndef DOCWIRE_RANGED_H
#define DOCWIRE_RANGED_H

#include "safety_policy.h"
#include "enforce.h"
#include <type_traits>

namespace docwire
{

/**
 * @brief A marker type to signify an unlimited bound in a ranged type.
 */
struct unlimited_t {};

/**
 * @brief A convenient instance of the unlimited_t marker.
 */
inline constexpr unlimited_t unlimited;

/**
 * @brief A wrapper for numeric types that enforces a range [Min, Max].
 *
 * In `strict` mode, the constructor checks if the value is within the specified
 * range and throws an exception if the invariant is violated.
 * In `relaxed` mode, this wrapper is a zero-cost abstraction with no runtime checks.
 *
 * @tparam Min The minimum allowed value (or unlimited).
 * @tparam Max The maximum allowed value (or unlimited).
 * @tparam T The underlying numeric type.
 * @tparam safety_level The safety policy to apply.
 */
template <auto Min, auto Max, typename T, safety_policy safety_level = default_safety_level>
class ranged
{
    static_assert(!std::is_same_v<std::remove_const_t<decltype(Min)>, unlimited_t> || !std::is_same_v<std::remove_const_t<decltype(Max)>, unlimited_t>,
                  "ranged must have at least one concrete bound; use the raw type instead of ranged<..., unlimited, unlimited>.");

    static_assert(std::is_same_v<std::remove_const_t<decltype(Min)>, unlimited_t> || std::is_convertible_v<decltype(Min), T>,
                  "The Min bound must be convertible to the ranged type T.");

    static_assert(std::is_same_v<std::remove_const_t<decltype(Max)>, unlimited_t> || std::is_convertible_v<decltype(Max), T>,
                  "The Max bound must be convertible to the ranged type T.");
public:
    /**
     * @brief Constructs a ranged value, enforcing the bounds in strict mode.
     * @param value The value to wrap.
     */
    ranged(T value) : m_value(value)
    {
        if constexpr (!std::is_same_v<std::remove_const_t<decltype(Min)>, unlimited_t>) {
            enforce<safety_level>(m_value >= static_cast<T>(Min), "Value is below the expected minimum", "value"_v = m_value, "min"_v = static_cast<T>(Min));
        }
        if constexpr (!std::is_same_v<std::remove_const_t<decltype(Max)>, unlimited_t>) {
            enforce<safety_level>(m_value <= static_cast<T>(Max), "Value is above the expected maximum", "value"_v = m_value, "max"_v = static_cast<T>(Max));
        }
    }

    /// Implicit conversion to the underlying type.
    operator T() const { return m_value; }
    T get() const { return m_value; }

private:
    T m_value;
};

/**
 * @brief A fluent alias for `ranged` that enforces a minimum value.
 * @example at_least<0, int> value;
 */
template <auto Min, typename T, safety_policy safety_level = default_safety_level>
using at_least = ranged<Min, unlimited, T, safety_level>;

/**
 * @brief A fluent alias for `ranged` that enforces a maximum value.
 * @example at_most<18, int> value;
 */
template <auto Max, typename T, safety_policy safety_level = default_safety_level>
using at_most = ranged<unlimited, Max, T, safety_level>;

/**
 * @brief A fluent alias for `ranged` that enforces an exact value.
 * @example exactly<100, int> value;
 */
template <auto Value, typename T, safety_policy safety_level = default_safety_level>
using exactly = ranged<Value, Value, T, safety_level>;

/**
 * @brief A fluent alias for `ranged` that enforces a non-negative value (>= 0).
 * @example non_negative<int> value;
 */
template <typename T, safety_policy safety_level = default_safety_level>
using non_negative = at_least<0, T, safety_level>;

} // namespace docwire

#endif // DOCWIRE_RANGED_H