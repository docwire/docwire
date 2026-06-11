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

#ifndef DOCWIRE_DIAGNOSTIC_CONTEXT_H
#define DOCWIRE_DIAGNOSTIC_CONTEXT_H

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace docwire
{

template <typename T>
concept context_tag = std::is_empty_v<T> && requires { { T::string() } -> std::convertible_to<std::string_view>; };

namespace diagnostic_context
{

// TODO: This stores context items by value, which is safe for exceptions but does not support
// non-copyable types. A future enhancement could be to detect non-copyable types at compile time
// and serialize them on the spot within make_error, while still storing copyable types by value.
/**
 * @brief Creates a context item from a named variable.
 * @details This is the general-purpose overload for creating a diagnostic context item.
 * It captures the variable's name and its value, returning them as a `std::pair`.
 * This is used by macros like `log_entry` and `make_error` to capture context.
 * @tparam T The type of the value being captured.
 * @param name The name of the variable.
 * @param v The value of the variable.
 * @return A `std::pair` containing the variable's name and its value.
 */
template<typename T>
auto make_context_item(const char* name, T&& v) -> std::pair<std::string, std::decay_t<T>>
{
	return {name, std::forward<T>(v)};
}

/**
 * @brief Creates a context item from a tag.
 * @details This overload is selected for types that satisfy the `context_tag` concept.
 * It discards the variable name and returns the tag object itself. This allows tags
 * to be passed through the diagnostic system without being wrapped in a name-value pair.
 * @tparam T A type that satisfies the `context_tag` concept.
 * @param name The name of the variable (discarded).
 * @param v The tag object.
 * @return The tag object, passed through.
 */
template <context_tag T>
T make_context_item(const char* name, T&& v)
{
	return std::forward<T>(v);
}

/**
 * @brief Creates a context item from a string literal.
 * @details This overload is selected for string literals. It discards the variable
 * name and returns the string literal directly as a `const char*`. This allows
 * anonymous strings to be passed as context without being wrapped in a name-value pair.
 * @param name The name of the variable (discarded).
 * @param v The string literal.
 * @return The string literal as a `const char*`.
 */
template <size_t N>
const char* make_context_item(const char* name, const char (&v)[N])
{
	return v;
}

} // namespace diagnostic_context

#define DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE_ELEM(r, data, i, elem) \
    BOOST_PP_COMMA_IF(i) docwire::diagnostic_context::make_context_item(BOOST_PP_STRINGIZE(elem), elem)

#define DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(...) \
    __VA_OPT__(BOOST_PP_SEQ_FOR_EACH_I(DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE_ELEM, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define DOCWIRE_DIAGNOSTIC_CONTEXT_GET_TYPE_ELEM(r, data, i, elem) \
    BOOST_PP_COMMA_IF(i) decltype(docwire::diagnostic_context::make_context_item(BOOST_PP_STRINGIZE(elem), elem))

#define DOCWIRE_DIAGNOSTIC_CONTEXT_GET_TYPES(...) \
    __VA_OPT__(BOOST_PP_SEQ_FOR_EACH_I(DOCWIRE_DIAGNOSTIC_CONTEXT_GET_TYPE_ELEM, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

} // namespace docwire

#endif // DOCWIRE_DIAGNOSTIC_CONTEXT_H