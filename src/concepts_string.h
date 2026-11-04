/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONCEPTS_STRING_H
#define DF0_CONCEPTS_STRING_H

#include <concepts>
#include <string_view>
#include <type_traits>

namespace df0
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

} // namespace df0

#endif // DF0_CONCEPTS_STRING_H
