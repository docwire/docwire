/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_TYPEINDEX_H
#define DF0_SERIALIZATION_TYPEINDEX_H

#include "core_export.h"
#include "serialization_base.h"
#include "type_name_base.h"
#include <typeindex>

namespace df0::serialization
{

template <>
struct serializer<std::type_index>
{
    value full(const std::type_index& t) const
    {
        return df0::type_name::from_type_index(t);
    }
    value typed_summary(const std::type_index& t) const { return decorate_with_typeid(this->full(t), type_name::pretty<std::type_index>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_TYPEINDEX_H
