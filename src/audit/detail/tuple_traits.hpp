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

#ifndef DF0_AUDIT_DETAIL_TUPLE_TRAITS_HPP
#define DF0_AUDIT_DETAIL_TUPLE_TRAITS_HPP

#include <tuple>
#include <type_traits>

namespace df0::audit::detail
{

template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... Args>
struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

} // namespace df0::audit::detail

#endif
