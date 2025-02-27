/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_NESTED_EXCEPTION_H
#define DOCWIRE_NESTED_EXCEPTION_H

#include <exception>

namespace docwire::errors
{

/**
 * @brief A template class that simplifies the creation of nested exceptions.
 *
 * In C++, creating a nested exception using `std::nested_exception` requires throwing and catching an inner exception,
 * and then using `std::throw_with_nested` to create the nested exception. This template class, together with
 * `make_nested` macro provides a more straightforward way to create nested exceptions without the need for
 * throwing and catching.
 *
 * @tparam T The type of the outer exception.
 *
 * @see make_nested
 * @see std::nested_exception
 */
template<typename T>
class nested : virtual public std::nested_exception, public T
{
public:

	/**
     * @brief Constructs a `nested` object from an rvalue reference to an object of type `T`.
	 *
	 * Outer exception is constructed from the rvalue reference.
	 * Inner exception is captured using std::current_exception() and stored in the std::nested_exception base class.
     *
     * @param t An rvalue reference to an object of type `T`.
     */
    nested(T&& t)
        : T(std::forward<T>(t))
    {
    }

	/**
     * @brief Constructs a `nested` object from a const lvalue reference to an object of type `T`.
     *
	 * Outer exception is constructed from the lvalue reference.
	 * exception is captured using std::current_exception() and stored in the std::nested_exception base class.
	 *
     * @param t A const lvalue reference to an object of type `T`.
     */
    nested(const T& t)
        : T(t)
    {
    }
};

/**
 * @brief Creates a nested exception from an inner exception and an outer exception.
 *
 * This function creates a nested exception by throwing the inner exception and catching it, then using the outer exception
 * to create a nested exception.
 *
 * @tparam Inner The type of the inner exception.
 * @tparam Outer The type of the outer exception.
 *
 * @param inner The inner exception to be nested. Can be exception object or std::exception_ptr.
 * @param outer The outer exception to wrap the inner exception.
 *
 * @return A nested exception object containing the inner and outer exceptions.
 *
 * @see nested
 * @see std::nested_exception
 * @see std::exception_ptr
 */
template <typename Inner, typename Outer>
auto make_nested(Inner&& inner, Outer&& outer)
{
	try
	{
		if constexpr (std::is_same_v<std::remove_cvref_t<Inner>, std::exception_ptr>)
			std::rethrow_exception(std::forward<Inner>(inner));
		else
			throw std::forward<Inner>(inner);
	}
	catch(...)
	{
		return nested<std::remove_cvref_t<Outer>>(std::forward<Outer>(outer));
	}
}

/**
 * @brief Creates a nested exception from inner exception and multiple outer exceptions.
 *
 * This function creates a nested exception by recursively calling `make_nested` with the inner exception and the outer exceptions.
 *
 * @tparam Inner The type of the inner exception.
 * @tparam Outer The type of the outer exception.
 * @tparam Rest The types of additional outer exceptions (additional layers of nesting).
 *
 * @param inner The inner exception to be nested. Can be exception object or std::exception_ptr.
 * @param outer The outer exception to wrap the inner exception.
 * @param rest Additional outer exceptions to add additional layers of nesting.
 *
 * @return A nested exception object containing whole layers of nested exceptions.
 *
 * @see nested
 * @see std::nested_exception
 * @see std::exception_ptr
 */
template <typename Inner, typename Outer, typename... Rest>
auto make_nested(Inner&& inner, Outer&& outer, Rest&&... rest)
{
	return make_nested(make_nested(std::forward<Inner>(inner), std::forward<Outer>(outer)), std::forward<Rest>(rest)...);
}

/**
 * @brief Creates a pointer to a nested exception from an inner exception and an outer exception.
 *
 * This function creates a pointer to a nested exception by recursively calling `make_nested` with
 * the inner exception and the outer exceptions and then converting the result to a std::exception_ptr.
 *
 * @tparam Inner The type of the inner exception.
 * @tparam Outer The type of the outer exception.
 * @tparam Rest The types of additional outer exceptions (additional layers of nesting).
 *
 * @param inner The inner exception to be nested. Can be exception object or std::exception_ptr.
 * @param outer The outer exception to wrap the inner exception.
 * @param rest Additional outer exceptions to add additional layers of nesting.
 *
 * @return A pointer to a nested exception object containing whole layers of nested exceptions.
 *
 * @see nested
 * @see std::nested_exception
 * @see std::exception_ptr
 */
template <typename Inner, typename Outer, typename... Rest>
std::exception_ptr make_nested_ptr(Inner&& inner, Outer&& outer, Rest&&... rest)
{
	return std::make_exception_ptr(make_nested(std::forward<Inner>(inner), std::forward<Outer>(outer), std::forward<Rest>(rest)...));
}

} // namespace docwire::errors

#endif
