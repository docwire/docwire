/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_EXCEPTION_H
#define DF0_SERIALIZATION_EXCEPTION_H

#include "serialization_base.h"
#include <exception>

namespace df0::serialization
{

template<typename T> requires std::is_base_of_v<std::exception, T>
struct serializer<T>
{
    value full(const T& e) const { return object{{{"what", e.what()}}}; }
    value typed_summary(const T& e) const { return decorate_with_typeid(full(e), type_name::pretty<T>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_EXCEPTION_H
