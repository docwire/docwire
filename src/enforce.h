/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ENFORCE_H
#define DF0_ENFORCE_H

#include "safety_policy.h"
#include "with_source_location.h"
#include "make_error.h"
#include "error_tags.h"
#include "debug_assert.h"

namespace df0
{

/**
 * @brief Enforces a condition based on the safety policy.
 * 
 * If the condition is false:
 * - In `strict` mode: Throws a `df0::error` exception with the `df0::errors::program_logic` tag attached and the provided context.
 * - In `relaxed` mode: Triggers a `debug_assert`, which terminates in debug builds but does nothing in release builds.
 * 
 * @tparam safety_level The safety policy to apply (default is strict).
 * @param condition The condition to check.
 * @param context Additional context information to include in the error/assertion.
 * @sa safety_policy
 * @sa debug_assert
 */
template<safety_policy safety_level = default_safety_level, typename... Context>
void enforce(detail::with_source_location<bool> condition, Context&&... context)
{
    if (!condition.value)
    {
        if constexpr (safety_level == strict) {
            auto context_tuple = std::make_tuple(std::forward<Context>(context)..., errors::program_logic{});
                throw errors::make_error_from_tuple(condition.location, std::move(context_tuple));
        } else { // relaxed
            debug_assert(condition, "Contract violation", std::forward<Context>(context)..., errors::program_logic{});
        }
    }
}

}

#endif // DF0_ENFORCE_H