/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_FILE_EXTENSION_H
#define DF0_SERIALIZATION_FILE_EXTENSION_H

#include "file_extension.h"
#include "serialization_base.h"

namespace df0::serialization
{

/**
 * @brief Specialization for `file_extension`.
 */
template <>
struct serializer<file_extension>
{
    value full(const file_extension& ext) const
    {
        return object{{{"value", ext.string()}}};
    }
    value typed_summary(const file_extension& ext) const { return decorate_with_typeid(full(ext), type_name::pretty<file_extension>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_FILE_EXTENSION_H
