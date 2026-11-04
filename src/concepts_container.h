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
