/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "unique_identifier_globals.h"

#include <atomic>
#include <cstddef>

namespace df0::detail
{

size_t next_unique_identifier()
{
    static std::atomic<size_t> counter{0};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

} // namespace df0::detail
