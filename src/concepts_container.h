/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONCEPTS_CONTAINER_H
#define DF0_CONCEPTS_CONTAINER_H

#include <iterator>
#include <type_traits>

namespace df0
{

/**
 * @brief Concept to detect if a type is a container (iterable and not self-recursive).
 */
template<typename T>
concept container = requires(const T& t) {
    { std::begin(t) } -> std::input_iterator;
    { std::end(t) } -> std::input_iterator;
    requires !std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<typename std::iterator_traits<decltype(std::begin(t))>::value_type>>;
};

} // namespace df0

#endif // DF0_CONCEPTS_CONTAINER_H
