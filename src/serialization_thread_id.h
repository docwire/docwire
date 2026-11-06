/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_THREAD_ID_H
#define DF0_SERIALIZATION_THREAD_ID_H

#include "core_export.h"
#include "serialization_base.h"
#include <sstream>
#include <thread>

namespace df0::serialization
{

template <>
struct serializer<std::thread::id>
{
    value full(const std::thread::id& i) const
    {
        std::ostringstream s;
        s << i;
        return s.str();
    }
    value typed_summary(const std::thread::id& i) const { return decorate_with_typeid(full(i), type_name::pretty<std::thread::id>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_THREAD_ID_H
