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

#ifndef DOCWIRE_DIAGNOSTIC_MESSAGE_H
#define DOCWIRE_DIAGNOSTIC_MESSAGE_H

#include "core_export.h"
#include <exception>
#include <string>
#include <typeinfo>
#include "error.h"

namespace docwire::errors
{

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
 */
inline std::string diagnostic_message(const std::exception& e)
{
	auto quote = [](const std::string& s) { return "\"" + s + "\""; };

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

	try
	{
		const errors::base& error = dynamic_cast<const errors::base&>(e);
		if (message.empty())
		{
			message += "Error: " + (error.context_count() > 0 ? quote(error.context_string(0)) : quote(e.what())) + "\n";
			message += "in " + std::string{error.location.function_name()} + "\n";
			message += "at " + std::string{error.location.file_name()} + ":" + std::to_string(error.location.line()) + "\n";
			for (size_t i = 1; i < error.context_count(); ++i)
			{
				message += "with context " + quote(error.context_string(i)) + "\n";
			}
		}
		else
		{
			message += "wrapping at: " + std::string{error.location.function_name()} + "\n";
			message += "at " + std::string{error.location.file_name()} + ":" + std::to_string(error.location.line()) + "\n";
			for (size_t i = 0; i < error.context_count(); ++i)
				message += "with context " + quote(error.context_string(i)) + "\n";
		}
	}
	catch (const std::bad_cast&)
	{
		message += "Error: " + quote(e.what()) + "\n";
		message += "No location information available\n";
	}
	return message;
}

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
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

} // namespace docwire::errors

#endif // DOCWIRE_DIAGNOSTIC_MESSAGE_H
