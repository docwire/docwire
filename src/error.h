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

#ifndef DOCWIRE_ERROR_H
#define DOCWIRE_ERROR_H

#include <exception>
#include <string>
#if __has_include(<source_location>) && (!defined(__clang__) || __clang_major__ >= 16) // https://github.com/llvm/llvm-project/issues/56379
	#define USE_STD_SOURCE_LOCATION 1
#else
	#warning "Cannot use std::source_location"
	#define USE_STD_SOURCE_LOCATION 0
#endif
#if USE_STD_SOURCE_LOCATION
	#include <source_location>
#else
	#include <boost/assert/source_location.hpp>
#endif
#include <sstream>
#include <utility>

/**
 * @brief Provides features for reporting and handling errors with context data using nested exceptions.
 */
namespace docwire::errors
{

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& s, const std::pair<T1, T2>& p)
{
	s << p.first << ": " << p.second;
	return s;
}

#if USE_STD_SOURCE_LOCATION
	using source_location = std::source_location;
	#define DOCWIRE_CURRENT_LOCATION() std::source_location::current()
#else
	using source_location = boost::source_location;
	#if !defined(__clang__) // https://github.com/llvm/llvm-project/issues/56379
		#define DOCWIRE_CURRENT_LOCATION() BOOST_CURRENT_LOCATION
	#else
		#define DOCWIRE_CURRENT_LOCATION() boost::source_location(__builtin_FILE(), __builtin_LINE(), __builtin_FUNCTION(), __builtin_COLUMN())
	#endif
#endif
#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
	#define current_location DOCWIRE_CURRENT_LOCATION
#endif

/**
 * @brief Base class for all exceptions in the SDK.
 *
 * This class provides a foundation for exceptions in the SDK, combining the ability to throw any object as an exception
 * with compatibility with the standard C++ exception class, std::exception.
 *
 * The class is designed to hold context information of any type, such as error messages, error tags, or important context
 * values. This information can be stored in the form of a string, a name-value pair, or any other C++ type.
 *
 * In addition to the context information, this class also stores the source location where the exception was thrown.
 * This information is useful for debugging and error reporting purposes.
 *
 * @note This class is intended to be used as a base class for other exception classes in the SDK. It should not be used
 *       directly for throwing exceptions. Instead, use the errors::impl template or the make_error macro to create
 *       derived exception classes.
 *
 * @note The what() method returns the exception type instead of a formatted message.
 *       This is because pre-formatting error messages in exceptions can have significant drawbacks,
 *       including the potential for confidential data to be leaked without the application's control.
 *       To avoid this, use the errors::diagnostic_message function instead.
 *
 * @code
 * try {
 * 	// SDK code that may throw an error
 * } catch (const docwire::errors::base& e) {
 *  std::cerr << e.what() << std::endl; // Print only the type name of the exception
 *  std::cerr << e.context_type().name() << std::endl; // Print the context information type name
 *  std::cerr << e.context_string() << std::endl; // Print the context information stringified
 *  std::cerr << errors::diagnostic_message(e) << std::endl; // Print the diagnostic message including nested exceptions
 * }
 * @endcode
 *
 * @see errors::impl
 * @see errors::make_error
 * @see errors::diagnostic_message
 */
struct base : public std::exception
{
	/**
	 * @brief The source location where the exception was thrown.
	 */
	errors::source_location source_location;

	/**
	 * @brief Constructs a base object with the current source location.
	 *
	 * @param location The source location of the exception (initialized by current location by default).
	 */
	base(const errors::source_location& location) : source_location(location) {}

	/**
	 * @brief Get the type information of the context.
	 *
	 * @return The type information of the context.
	 * @see context_string
	 */
	virtual std::type_info const& context_type() const noexcept = 0;

	/**
	 * @brief Get the string representation of the context.
	 *
	 * @return The string representation of the context.
	 * @see context_type
	 */
	virtual std::string context_string() const = 0;

	/**
	 * @brief Get the exception type.
	 *
	 * This method is provided for compatibility with the standard C++ exception class.
	 * It returns the type name of the exception instead of a formatted message for security reasons.
	 *
	 * @return The exception type.
	 * @see diagnostic_message
	 */
	virtual const char* what() const noexcept override
	{
		return typeid(*this).name();
	}
};

/**
 * @brief Implementation of the error class for a specific context type.
 *
 * This class is used to throw exceptions with additional context information.
 * It provides a way to throw exceptions with a specific context type, such as:
 *
 * @code
 * throw errors::impl{"message"}
 * throw errors::impl{errors::network_error{}}
 * throw errors::impl{std::pair<"key", "value"}}
 * @endcode
 *
 * The error::impl template can be used directly, but the make_error macro offers more features,
 * such as stringification of expressions.
 *
 * The class stores an instance of the context type and provides methods to get the type and string
 * representation of the context.
 *
 * @tparam T The type of the context.
 *
 * @see errors::base
 * @see errors::make_error
 * @see errors::diagnostic_message
 */
template <typename T>
struct impl : public base
{
	T context;
	
	/**
	 * @brief Constructor for the impl class.
	 *
	 * @param context The context to be stored.
	 * @param location The source location of the exception (initialized by current location by default).
	 */
	impl(const T& context, const errors::source_location& location = current_location())
		: base(location), context(context)
	{
	}

	/**
	 * @brief Get the type information of the context.
	 *
	 * @return The type information of the context.
	 * @see context_string
	 * @see context
	 */
	std::type_info const& context_type() const noexcept override
	{
		return typeid(T);
	}

	/**
	 * @brief Get the string representation of the context.
	 *
	 * The method attempts to convert the context to a string using various methods,
	 * depending on the type T.
	 *
	 * @return The string representation of the context.
	 * @see context_type
	 * @see context
	 */
	std::string context_string() const override
	{
		if constexpr (requires { context.string(); })
			return context.string();
		else if constexpr (requires { std::to_string(context); })
			return std::to_string(context);
		else
		{
			std::ostringstream s;
			s << context;
			return s.str();
		}
	}
};

template <typename T>
std::pair<std::string, T> convert_to_context(const std::string& name, const T& v)
{
	return std::pair<std::string, T>{name, v};
}

template <size_t N>
const char* convert_to_context(const std::string& name, const char (&v)[N])
{
	return v;
}

} // namespace docwire::errors

#endif
