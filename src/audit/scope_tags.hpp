/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_SCOPE_TAGS_H
#define DF0_AUDIT_SCOPE_TAGS_H

#include <string_view>

namespace df0::audit
{

struct scope_enter
{
    static constexpr std::string_view string() { return "scope_enter"; }
};

struct scope_exit
{
    static constexpr std::string_view string() { return "scope_exit"; }
};

} // namespace df0::audit

#endif
