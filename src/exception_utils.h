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

#ifndef DOCWIRE_EXCEPTION_UTILS_H
#define DOCWIRE_EXCEPTION_UTILS_H

#include "error.h"
#include <string>

namespace docwire::errors
{

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
 *
 * This function recursively traverses the nested exceptions chain and returns a string representation of the entire chain.
 *
 * @param e The root exception of the nested exceptions chain.
 * @return A string representation of the nested exceptions chain.
 */
inline std::string diagnostic_message(const std::exception& e)
{
	std::string message;
	try
	{
		std::rethrow_if_nested(e);
	}
	catch (const std::exception& nested_ex)
	{
		message = diagnostic_message(nested_ex);
	}
	catch (...)
	{
		message = "Unknown error\n";
	}
	message += std::string{message.empty() ? "Error" : "with context"} + " \"";
	try
	{
		const errors::base& error = dynamic_cast<const errors::base&>(e);
		message +=
			error.context_string() + "\"\nin " +
			error.source_location.function_name() + "\nat " +
			error.source_location.file_name() + ":" +
				std::to_string(error.source_location.line()) + "\n";
	}
	catch (const std::bad_cast&)
	{
		message += std::string{e.what()} + "\"\nNo location information available\n";
	}
	return message;
}

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
 *
 * This function recursively traverses the nested exceptions chain and returns a string representation of the entire chain.
 *
 * @param eptr Pointer to the root exception of the nested exceptions chain.
 * @return A string representation of the nested exceptions chain.
 */
inline std::string diagnostic_message(std::exception_ptr eptr)
{
	try
	{
		std::rethrow_exception(eptr);
	}
	catch(const std::exception& e)
	{
		return diagnostic_message(e);
	}
	catch (...)
	{
		return "Unknown error\n";
	}
}

/**
 * @brief Checks if the given nested exceptions chain contains a specific type of context.
 *
 * This function recursively traverses the nested exceptions chain and returns true if
 * the specified type of context is found.
 *
 * @param e The root exception of the nested exceptions chain.
 * @return True if the nested exceptions chain contains the specified type of context, false otherwise.
 */
template <typename T>
bool contains_type(const std::exception& e)
{
	std::string message;
	try
	{
		const errors::base& error = dynamic_cast<const errors::base&>(e);
		if (error.context_type() == typeid(T))
			return true;
	}
	catch (const std::bad_cast&)
	{
		return false;
	}
	try
	{
		std::rethrow_if_nested(e);
	}
	catch (const std::exception& nested_ex)
	{
		return contains_type<T>(nested_ex);
	}
	catch (...)
	{
		return false;
	}
	return false;
}

} // namespace docwire::errors

#endif
