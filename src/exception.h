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
		This class is implementation of std::exception, which is used by DocWire.
		In this implementation, errors can be formed in "stack".

		In order to create exception just call:
		\code
		throw Exception("First error");
		\endcode

		You can catch exception and add one more error:
		\code
		catch (Exception& ex)
		{
			ex.appendError("Next error message");
			throw;
		}
		\endcode

		or you can catch exception and get "backtrace":
		\code
		catch (Exception& ex)
		{
			docwire_log(error) << ex.getBacktrace();
		}
		\endcode
	**/
	class DllExport Exception : public std::exception
	{
		private:
			struct Implementation;
			Implementation* impl;

		public:

			Exception() noexcept;

			/**
				The constructor.
				\param first_error_message first error message (gives information about cause of an error).
			**/
			explicit Exception(const std::string& first_error_message) noexcept;

			Exception(const Exception& ex) noexcept;

			~Exception() noexcept;

			Exception& operator = (const Exception& ex) noexcept;

			const char* what(){ return "docwire_exception"; }

			/**
				Returns a string with all error messages. Each error message is numbered and separated by "\n".
				Suppose we have thrown an exception:
				\code
				throw Exception("First error message");
				\endcode
				Next, we have added one more error:
				\code
				ex.appendError("Second error message");
				\endcode
				In the result getBacktrace will return a string: "Backtrace:\n1. First error message\n2. Second error message\n"
			**/
			std::string getBacktrace();

			/**
				Adds one more error message.
			**/
			void appendError(const std::string& error_message);

			/**
				returns an iterator to the first error message.
			**/
			std::list<std::string>::iterator getErrorIterator() const;

			/**
				Returns a number of error messages.
			**/
			size_t getErrorCount() const;
	};

class DllExport LogicError : public std::logic_error
{
public:
	LogicError(const std::string& message);
	LogicError(const std::string& message, const std::exception& nested);
};

class DllExport RuntimeError : public std::runtime_error
{
public:
	RuntimeError(const std::string& message);
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
