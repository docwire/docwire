/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_ENUM_H
#define DF0_SERIALIZATION_ENUM_H

#include "serialization_base.h"
#include <magic_enum/magic_enum.hpp>

namespace df0::serialization
{

template <typename T> requires std::is_enum_v<T>
struct serializer<T>
{
    value full(const T& value) const { return std::string{magic_enum::enum_name(value)}; }
    value typed_summary(const T& value) const { return decorate_with_typeid(full(value), type_name::pretty<T>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_ENUM_H
