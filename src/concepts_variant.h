/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONCEPTS_VARIANT_H
#define DF0_CONCEPTS_VARIANT_H

#include <variant>
#include <type_traits>

namespace df0
{

template <typename T, typename Variant>
struct is_variant_alternative_trait : std::false_type {};

template <typename T, typename... Us>
struct is_variant_alternative_trait<T, std::variant<Us...>> : std::bool_constant<(std::is_same_v<T, Us> || ...)> {};

template <typename T, typename Variant>
concept variant_alternative = is_variant_alternative_trait<T, Variant>::value;

}

#endif // DF0_CONCEPTS_VARIANT_H
