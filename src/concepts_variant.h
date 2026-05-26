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

#ifndef DOCWIRE_CONCEPTS_VARIANT_H
#define DOCWIRE_CONCEPTS_VARIANT_H

#include <variant>
#include <type_traits>

namespace docwire
{

template <typename T, typename Variant>
struct is_variant_alternative_trait : std::false_type {};

template <typename T, typename... Us>
struct is_variant_alternative_trait<T, std::variant<Us...>> : std::bool_constant<(std::is_same_v<T, Us> || ...)> {};

template <typename T, typename Variant>
concept variant_alternative = is_variant_alternative_trait<T, Variant>::value;

}

#endif // DOCWIRE_CONCEPTS_VARIANT_H
