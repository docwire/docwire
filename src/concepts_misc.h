/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONCEPTS_MISC_H
#define DF0_CONCEPTS_MISC_H

#include <type_traits>

namespace df0
{

/**
 * @brief Concept for strong type aliases that wrap a single public member `v`.
 */
template <typename T>
concept strong_type_alias = requires(T value) { value.v; };

/**
 * @brief Concept to detect if a type is dereferenceable like a pointer.
 */
 template<typename T>
 concept dereferenceable = requires(const T& t) { *t; !t; };

/**
 * @brief Concept for empty structs.
 */
template<typename T>
concept empty = std::is_empty_v<T>;

} // namespace df0

#endif // DF0_CONCEPTS_MISC_H
