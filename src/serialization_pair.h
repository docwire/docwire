/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_PAIR_H
#define DF0_SERIALIZATION_PAIR_H

#include "serialization_base.h"
#include <utility>

namespace df0::serialization
{

template <typename T1, typename T2>
struct serializer<std::pair<T1, T2>>
{
    value full(const std::pair<T1, T2>& pair) const
    {
        return object{{
            {"first", serialization::full(pair.first)},
            {"second", serialization::full(pair.second)}
        }};
    }

    value typed_summary(const std::pair<T1, T2>& pair) const
    {
        return decorate_with_typeid(object{{
            {"first", serialization::typed_summary(pair.first)},
            {"second", serialization::typed_summary(pair.second)}
        }}, type_name::pretty<std::pair<T1, T2>>());
    }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_PAIR_H
