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

#ifndef DOCWIRE_EXCEPTION_H
#define DOCWIRE_EXCEPTION_H

#include <exception>
#include <list>
#include <string>
#include "defines.h"
#include <stdexcept>

namespace docwire
{

/**
 * @brief The exception class that is thrown when a logical error occurs.
 *
 * This exception class is derived from std::logic_error and should be used
 * whenever logical error occurs, e.g. when input data is invalid.
 *
 * Example:
 * \code
 * if (input_data.size() == 0)
 *     throw LogicError("Input data is empty");
 * \endcode
 */
class DllExport LogicError : public std::logic_error
{
public:
	/**
	 * @brief Constructs the exception object.
	 * @param message the exception message.
	 */
	LogicError(const std::string& message);
	/**
	 * @brief Constructs the exception object.
	 * @param message the exception message.
	 * @param nested nested exception.
	 */
	LogicError(const std::string& message, const std::exception& nested);
};

/**
 * @brief The exception class that is thrown when a runtime error occurs.
 *
 * This exception class is derived from std::runtime_error and should be used
 * whenever runtime error occurs, e.g. when there is not enough memory or
 * when network connection is lost.
 *
 * Example:
 * \code
 * if (!allocateMemory())
 *     throw RuntimeError("Not enough memory");
 * \endcode
 */
class DllExport RuntimeError : public std::runtime_error
{
public:
	/**
	 * @brief Constructs the exception object.
	 * @param message the exception message.
	 */
	RuntimeError(const std::string& message);
	/**
	 * @brief Constructs the exception object.
	 * @param message the exception message.
	 * @param nested nested exception.
	 */
	RuntimeError(const std::string& message, const std::exception& nested);
};


class EncryptedFileException : public docwire::RuntimeError
{
public:
	EncryptedFileException(const std::string& message) : docwire::RuntimeError(message) {}
	EncryptedFileException(const std::string& message, const std::exception& nested) : docwire::RuntimeError(message, nested) {}
};

#define DOCWIRE_EXCEPTION_DEFINE(Name, Base) \
	class DllExport Name : public Base \
	{ \
	public: \
		Name(const std::string& message) : Base(message) {} \
		Name(const std::string& message, const std::exception& nested) : Base(message, nested) {} \
	} \

} // namespace docwire

#endif
