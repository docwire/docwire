/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_MAKE_ERROR_H
#define DF0_MAKE_ERROR_H

#include "diagnostic_context.h"
#include "error.h" // IWYU pragma: keep
#include <tuple> // IWYU pragma: keep
#include <type_traits> // IWYU pragma: keep

namespace df0::errors
{

/**
 * @brief Creates an error object from a tuple of context information.
 * @param location The source location where the error occurred.
 * @param context_tuple A tuple containing context data.
 * @return The constructed error object.
 */
template<typename... Context>
auto make_error_from_tuple(const source_location& location, std::tuple<Context...>&& context_tuple)
{
    return std::apply([&](auto&&... args) {
        return errors::impl<std::remove_cvref_t<decltype(args)>...>(std::move(context_tuple), location);
    }, context_tuple);
}

/**
 * @brief Creates an exception_ptr containing an error object from a tuple of context information.
 * @param location The source location where the error occurred.
 * @param context_tuple A tuple containing context data.
 * @return An exception_ptr to the constructed error.
 */
template<typename... Context>
auto make_error_ptr_from_tuple(const source_location& location, std::tuple<Context...>&& context_tuple)
{
    return std::make_exception_ptr(make_error_from_tuple(location, std::move(context_tuple)));
}

} // namespace df0::errors

#define DF0_MAKE_ERROR_AT_LOCATION(explicit_location, ...) \
    ::df0::errors::make_error_from_tuple(explicit_location, std::make_tuple(DF0_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(__VA_ARGS__)))

#define DF0_MAKE_ERROR(...) \
    DF0_MAKE_ERROR_AT_LOCATION(df0::source_location::current() __VA_OPT__(,) __VA_ARGS__)

#define DF0_MAKE_ERROR_PTR(...) \
	std::make_exception_ptr(DF0_MAKE_ERROR(__VA_ARGS__))

#ifdef DF0_ENABLE_SHORT_MACRO_NAMES
#define make_error DF0_MAKE_ERROR
#define make_error_ptr DF0_MAKE_ERROR_PTR
#endif

#endif
