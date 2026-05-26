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

#ifndef DOCWIRE_MAKE_ERROR_H
#define DOCWIRE_MAKE_ERROR_H

#include "diagnostic_context.h"
#include "error.h" // IWYU pragma: keep
#include <tuple> // IWYU pragma: keep
#include <type_traits> // IWYU pragma: keep

namespace docwire::errors
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

} // namespace docwire::errors

#define DOCWIRE_MAKE_ERROR_AT_LOCATION(explicit_location, ...) \
    ::docwire::errors::make_error_from_tuple(explicit_location, std::make_tuple(DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(__VA_ARGS__)))

#define DOCWIRE_MAKE_ERROR(...) \
    DOCWIRE_MAKE_ERROR_AT_LOCATION(docwire::source_location::current() __VA_OPT__(,) __VA_ARGS__)

#define DOCWIRE_MAKE_ERROR_PTR(...) \
	std::make_exception_ptr(DOCWIRE_MAKE_ERROR(__VA_ARGS__))

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
#define make_error DOCWIRE_MAKE_ERROR
#define make_error_ptr DOCWIRE_MAKE_ERROR_PTR
#endif

#endif
