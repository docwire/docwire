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

#ifndef DOCWIRE_ERROR_H
#define DOCWIRE_ERROR_H

#include "core_export.h"
#include "diagnostic_context.h" // IWYU pragma: keep
#include <exception>
#include "serialization_pair.h" // IWYU pragma: keep
#include "stringification.h"
#include "source_location.h"
#include <tuple>
#include <utility>

/**
 * @brief Provides features for reporting and handling errors with context data using nested exceptions.
 * @see @ref handling_errors_and_warnings.cpp "handling errors and warnings example"
 */
namespace docwire::errors
{
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
 *  std::cerr << "Exception type: " << e.what() << std::endl;
 *  // Manually inspect context items:
 *  for (size_t i = 0; i < e.context_count(); ++i) {
 *      std::cerr << "  Context item " << i << ": " << e.context_string(i) << std::endl;
 *  }
 *  // Or print the full, user-friendly diagnostic message:
 *  std::cerr << errors::diagnostic_message(e) << std::endl;
 * }
 * @endcode
 *
 * @see errors::impl
 * @see errors::make_error
 * @see errors::diagnostic_message
 * @see @ref handling_errors_and_warnings.cpp "handling errors and warnings example"
 */
struct DOCWIRE_CORE_EXPORT base : public std::exception
{
	/// @brief The source location where the exception was thrown.
	source_location location;

	/**
	 * @brief Constructs a base object with the current source location.
	 *
	 * @param location The source location of the exception (initialized by current location by default).
	 */
	base(const source_location& location = source_location::current());

	/**
	 * @brief Get the type information of the context.
	 *
	 * @param index The index of the context item.
	 * @return The type information of the context item at the given index.
	 * @see context_string
	 */
	virtual std::type_info const& context_type(size_t index) const noexcept = 0;

	/**
	 * @brief Get the string representation of the context.
	 *
	 * @param index The index of the context item.
	 * @return The string representation of the context item at the given index.
	 * @see context_type
	 */
	virtual std::string context_string(size_t index) const = 0;

	/**
	 * @brief Get the number of context items.
	 */
	virtual size_t context_count() const noexcept = 0;

	/**
	 * @brief Get the exception type.
	 *
	 * This method is provided for compatibility with the standard C++ exception class.
	 * It returns the type name of the exception instead of a formatted message for security reasons.
	 *
	 * @return The exception type.
	 * @see diagnostic_message
	 */
	virtual const char* what() const noexcept override;
};

/**
 * @brief Implementation of the error class for a variadic number of context items.
 *
 * This class is used to throw exceptions with additional context information.
 * It can store multiple context items of different types.
 *
 * @code
 * // It is recommended to use the make_error macro instead of constructing impl directly.
 * throw make_error("A simple error message");
 *
 * std::string file_path = "/path/to/file";
 * throw make_error("File not found", errors::file_not_found{}, file_path);
 * @endcode
 *
 * While the `errors::impl` template can be used directly, it is strongly recommended to use the `make_error`
 * macro, which correctly handles context creation and source location capture.
 *
 * The class stores the context items in a tuple and provides methods to access them by index.
 *
 * @tparam T The types of the context items.
 *
 * @see errors::base
 * @see errors::make_error
 * @see errors::diagnostic_message
 * @see @ref handling_errors_and_warnings.cpp "handling errors and warnings example"
 */
template <typename... T>
struct impl : public base
{
private:
    template<size_t I>
    std::string context_string_impl() const
    {
        return stringify(std::get<I>(context));
    }

    template<size_t I>
    const std::type_info& context_type_impl() const noexcept
    {
        return typeid(std::get<I>(context));
    }

    template <size_t... Is>
    std::string context_string_at(size_t index, std::index_sequence<Is...>) const {
        using FuncType = std::string(impl::*)() const;
        static constexpr FuncType funcs[] = { &impl::template context_string_impl<Is>... };
        return (this->*funcs[index])();
    }

    template <size_t... Is>
    const std::type_info& context_type_at(size_t index, std::index_sequence<Is...>) const noexcept {
        using FuncType = const std::type_info&(impl::*)() const noexcept;
        static constexpr FuncType funcs[] = { &impl::template context_type_impl<Is>... };
        return (this->*funcs[index])();
    }

public:
	/**
	 * @brief A tuple holding all context items provided when the error was created.
	 */
	std::tuple<T...> context;
	
	/**
	 * @brief Constructs an error object from a tuple of context items.
	 *
	 * @param context_tuple A tuple containing the context to be stored.
	 * @param location The source location of the exception (initialized by current location by default).
	 */
	explicit impl(const std::tuple<T...>& context_tuple, const source_location& location = source_location::current())
		: base(location), context(context_tuple)
	{
	}

	/**
	 * @brief Get the type information of the context.
	 *
	 * @param index The index of the context item.
	 * @return The type information of the context item at the given index.
	 * @see context_string
	 * @see context
	 */
	std::type_info const& context_type(size_t index) const noexcept override
	{
		return context_type_at(index, std::make_index_sequence<sizeof...(T)>{});
	}

	/**
	 * @brief Get the string representation of the context.
	 *
	 * @param index The index of the context item.
	 * @return The string representation of the context item at the given index.
	 * @see context_type
	 * @see context
	 */
	std::string context_string(size_t index) const override
	{
		return context_string_at(index, std::make_index_sequence<sizeof...(T)>{});
	}

	/**
	 * @brief Get the number of context items.
	 * @return The number of context items stored in this error object.
	 */
	size_t context_count() const noexcept override
	{
		return sizeof...(T);
	}
};

} // namespace docwire::errors

#endif
