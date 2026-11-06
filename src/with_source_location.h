/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_WITH_SOURCE_LOCATION_H
#define DF0_WITH_SOURCE_LOCATION_H

#include "source_location.h"

namespace df0::detail
{
    /**
     * @brief Helper struct to capture the source location of a call site.
     * This is implicitly constructed from a value of type T.
     * @tparam T The type of the value being wrapped.
     */
    template <typename T>
    struct with_source_location
    {
        T value;
        source_location location;

        constexpr with_source_location(T val, const source_location& loc = source_location::current())
            : value(val), location(loc) {}
    };
}

#endif // DF0_WITH_SOURCE_LOCATION_H