/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_MESSAGE_H
#define DF0_SERIALIZATION_MESSAGE_H

#include "message.h"
#include "serialization_base.h"

namespace df0::serialization
{

template <>
struct serializer<message_base>
{
    value full(const message_base& msg) const
    {
        return object{{{"object_type", type_name::from_type_index(msg.object_type_info())}}};
    }
    value typed_summary(const message_base& msg) const { return decorate_with_typeid(full(msg), type_name::pretty<message_base>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_MESSAGE_H
