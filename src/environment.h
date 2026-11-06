/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ENVIRONMENT_H
#define DF0_ENVIRONMENT_H

#include "core_export.h"
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

namespace df0::environment
{
    inline std::optional<std::string> get(std::string_view name)
    {
        const std::string name_str{name};
        const char* value = std::getenv(name_str.c_str());
        if (value)
        {
            return std::string{value};
        }
        return std::nullopt;
    }
}

#endif // DF0_ENVIRONMENT_H
