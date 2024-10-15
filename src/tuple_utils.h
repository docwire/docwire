/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_TUPLE_UTILS_H
#define DOCWIRE_TUPLE_UTILS_H

#include <tuple>

/**
 * @brief Provides transformations for tuples.
 *
 * This namespace contains transformations that can be used to manipulate
 * tuple types.
 */
namespace docwire::tuple_utils
{

/**
 * @brief Returns subrange of a tuple.
 * 
 * @param t The tuple to return the subrange of.
 * @tparam Start The start index of the subrange.
 * @tparam Length The length of the subrange.
 * @returns The subrange of the tuple.
 */
template <std::size_t Start, std::size_t Length, typename... Ts>
auto subrange(const std::tuple<Ts...>& tuple) {
    if constexpr (Length == 0) {
      return std::tuple<>();
    } else {
      static_assert(Start < sizeof...(Ts), "Start index out of range");
      static_assert(Start + Length <= sizeof...(Ts), "Length out of range");
      return std::tuple_cat(std::make_tuple(std::get<Start>(tuple)), subrange<Start + 1, Length - 1>(tuple));
    }
}

/**
 * @brief Type alias for the type of the result of subrange of a tuple.
 *
 * @tparam T The tuple type to return the subrange of.
 * @see subrange
 */
template <std::size_t Start, std::size_t Length, typename T>
using subrange_t = decltype(subrange<Start, Length>(std::declval<T>()));

/**
 * @brief Removes the first element from a tuple.
 *
 * This function provides a way to remove the first element from a tuple,
 * resulting in a new tuple with the remaining elements.
 *
 * @param tuple The tuple to remove the first element from.
 * @returns The new tuple with the first element removed.
 */
template <typename T>
auto remove_first(T&& tuple) {
  static_assert(std::tuple_size_v<T> > 0, "Cannot remove the first element of an empty tuple.");
  return subrange<1, std::tuple_size_v<T> - 1>(tuple);
}

/**
 * @brief Type alias for the type of the result of removing the first element from a tuple.
 *
 * @tparam T The tuple type to remove the first element from.
 * @see remove_first
 */
template <typename T>
using remove_first_t = decltype(remove_first(std::declval<T>()));

/**
 * @brief Removes the last element from a tuple.
 *
 * This function provides a way to remove the last element from a tuple,
 * resulting in a new tuple with the remaining elements.
 *
 * @param tuple The tuple to remove the last element from.
 * @returns The new tuple with the last element removed.
 */
template <typename T>
auto remove_last(T&& tuple) {
  static_assert(std::tuple_size_v<T> > 0, "Cannot remove the last element of an empty tuple.");
  return subrange<0, std::tuple_size_v<T> - 1>(tuple);
}

/**
 * @brief Type alias for the type of the result of removing the last element from a tuple.
 *
 * @tparam T The tuple type to remove the last element from.
 * @see remove_last
 */
template <typename T>
using remove_last_t = decltype(remove_last(std::declval<T>()));

/**
 * @brief Returns the first element of a tuple.
 * 
 * @param tuple The tuple to return the first element of.
 * @returns The first element of the tuple.
 */
template <typename T>
auto first_element(T&& tuple) {
  static_assert(std::tuple_size_v<T> > 0, "Cannot get the first element of an empty tuple.");
  return std::get<0>(tuple);
}

/**
 * @brief Type alias for the type of the first element of a tuple.
 * 
 * @tparam T The tuple type to return the first element of.
 * @see first_element
 */
template <typename T>
using first_element_t = decltype(first_element(std::declval<T>()));

/**
 * @brief Returns the last element of a tuple.
 *
 * @param tuple The tuple to return the last element of.
 * @returns The last element of the tuple.
 */
template <typename T>
auto last_element(T&& tuple) {
  static_assert(std::tuple_size_v<T> > 0, "Cannot get the last element of an empty tuple.");
  return std::get<std::tuple_size_v<T> - 1>(tuple);
}

/**
 * @brief Type alias for the type of the last element of a tuple.
 * 
 * @tparam T The tuple type to return the last element of.
 * @see last_element
 */
template <typename T>
using last_element_t = decltype(last_element(std::declval<T>()));

} // namespace docwire::tuple_utils

#endif //DOCWIRE_TUPLE_UTILS_H
