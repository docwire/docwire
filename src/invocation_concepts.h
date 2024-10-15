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

#ifndef DOCWIRE_INVOCATION_CONCEPTS_H
#define DOCWIRE_INVOCATION_CONCEPTS_H

#include "invocation_traits.h"

/**
 * @brief Provides concepts for working with invocable objects and pushable containers.
 *
 * This namespace contains concepts that can be used to constrain template parameters to invocable objects and pushable containers.
 * The concepts are built on top of the traits classes provided in the @ref docwire::invocation_traits namespace.
 */
namespace docwire::invocation_concepts
{

/**
 * @brief Checks if a type is invocable.
 *
 * A type is considered invocable if it can be called like a function.
 * This concept uses the @ref docwire::invocation_traits::is_invocable_v trait to check for invocability.
 *
 * @tparam T The type to check for invocability.
 */
template <typename T>
concept invocable = invocation_traits::is_invocable_v<T>;

/**
 * @brief Checks if a type is not invocable.
 *
 * A type is considered not invocable if it cannot be called like a function.
 * This concept is the negation of the @ref invocable concept.
 *
 * @tparam T The type to check for non-invocability.
 */
template <typename T>
concept not_invocable = not invocable<T>;

/**
 * @brief Checks if a type is pushable.
 *
 * A type is considered pushable if it provides a `push_back` member function that can be used to append elements to the container.
 * This concept uses a requires clause to check for the presence of the `push_back` member function.
 *
 * @tparam T The type to check for pushability.
 */
template <typename T>
concept pushable = requires(T&& t) {
  { t.push_back(std::declval<typename std::remove_reference_t<T>::value_type>()) };
};

} // namespace docwire::invocation_concepts

#endif //DOCWIRE_INVOCATION_CONCEPTS_H
