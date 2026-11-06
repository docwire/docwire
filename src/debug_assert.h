/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_DEBUG_ASSERT_H
#define DF0_DEBUG_ASSERT_H

#include "make_error.h"
#include "with_source_location.h"

// In debug builds (when NDEBUG is not defined), DF0_DEBUG_ASSERT checks a condition
// and if it's false, prints a message and terminates the program.
// In release builds (when NDEBUG is defined), it compiles to nothing.
#ifndef NDEBUG
#include "diagnostic_message.h"
#include <exception>
#include <iostream>
#include <cstdlib>

namespace df0::errors
{
    /**
     * @brief Terminates the program with a panic message in debug builds.
     */
    [[noreturn]] inline void panic(std::exception_ptr eptr)
    {
        std::cerr << "Terminating due to contract violation: " << diagnostic_message(eptr) << std::endl;
        std::abort();
    }
}
#endif // NDEBUG

namespace df0
{

#ifndef NDEBUG

/**
 * @brief Asserts a condition in debug builds.
 * 
 * In debug builds (NDEBUG not defined), if the condition is false, the program terminates
 * with a panic message containing the provided context.
 * In release builds, this function does nothing.
 * 
 * @param condition The condition to check.
 * @param context Additional context information to log if the assertion fails.
 */
template<typename... Context>
void debug_assert(detail::with_source_location<bool> condition, Context&&... context)
{
    if (!condition.value)
    {
        errors::panic(make_error_ptr_from_tuple(condition.location, std::make_tuple(std::forward<Context>(context)...)));
    }
}

#else

template<typename... Context>
constexpr void debug_assert(detail::with_source_location<bool> /*condition*/, Context&&...) {}

#endif // NDEBUG

} // namespace df0

#endif // DF0_DEBUG_ASSERT_H
