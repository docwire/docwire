/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_DATA_SOURCE_H
#define DF0_SERIALIZATION_DATA_SOURCE_H

#include "data_source.h"
#include "serialization_base.h"
#include "serialization_file_extension.h" // IWYU pragma: keep
#include "serialization_filesystem.h" // IWYU pragma: keep

namespace df0::serialization
{

template <>
struct serializer<data_source>
{
    value full(const data_source& data) const
    {
        return object{{
            {"path", serialization::full(data.path())},
            {"file_extension", serialization::full(data.file_extension())}
        }};
    }

    value typed_summary(const data_source& data) const
    {
        return decorate_with_typeid(object{{
            {"path", serialization::typed_summary(data.path())},
            {"file_extension", serialization::typed_summary(data.file_extension())}
        }}, type_name::pretty<data_source>());
    }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_DATA_SOURCE_H
