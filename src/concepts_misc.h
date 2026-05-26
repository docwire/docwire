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

#ifndef DOCWIRE_CONCEPTS_MISC_H
#define DOCWIRE_CONCEPTS_MISC_H

#include <type_traits>

namespace docwire
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

} // namespace docwire

#endif // DOCWIRE_CONCEPTS_MISC_H
