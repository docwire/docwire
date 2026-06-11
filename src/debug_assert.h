/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_DEBUG_ASSERT_H
#define DOCWIRE_DEBUG_ASSERT_H

#include "make_error.h"
#include "with_source_location.h"

// In debug builds (when NDEBUG is not defined), DOCWIRE_DEBUG_ASSERT checks a condition
// and if it's false, prints a message and terminates the program.
// In release builds (when NDEBUG is defined), it compiles to nothing.
#ifndef NDEBUG
namespace docwire::errors
{
    /**
     * @brief Terminates the program with a panic message in debug builds.
     */
    [[noreturn]] DOCWIRE_CORE_EXPORT void panic(std::exception_ptr eptr);
}
#endif // NDEBUG

namespace docwire
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

} // namespace docwire

#endif // DOCWIRE_DEBUG_ASSERT_H