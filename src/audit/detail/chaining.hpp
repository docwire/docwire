/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_DETAIL_CHAINING_HPP
#define DF0_AUDIT_DETAIL_CHAINING_HPP

#include <type_traits>

namespace df0::audit::detail
{

struct empty {};

template<typename Next>
using next_storage_t = std::conditional_t<std::is_void_v<Next>, empty, Next>;

} // namespace df0::audit::detail

#endif
