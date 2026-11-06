/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SERIALIZATION_FILESYSTEM_H
#define DF0_SERIALIZATION_FILESYSTEM_H

#include <filesystem>
#include "serialization_base.h"

namespace df0::serialization
{

template <>
struct serializer<std::filesystem::path>
{
    value full(const std::filesystem::path& p) const
    {
        return p.string();
    }
    value typed_summary(const std::filesystem::path& p) const { return decorate_with_typeid(full(p), type_name::pretty<std::filesystem::path>()); }
};

} // namespace df0::serialization

#endif // DF0_SERIALIZATION_FILESYSTEM_H
