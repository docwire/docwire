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

#include "exception.h"

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>
#include <list>
#include "misc.h"

namespace docwire
{

struct Exception::Implementation
{
	std::list<std::string> m_errors;
};

Exception::Exception() noexcept
{
	impl = new Implementation;
}

Exception::Exception(const std::string &first_error_message) noexcept
{
	impl = new Implementation;
	impl->m_errors.push_back(first_error_message);
}

Exception::Exception(const Exception &ex) noexcept
{
	impl = new Implementation;
	*impl = *ex.impl;
}

Exception::~Exception() noexcept
{
	if (impl)
		delete impl;
}

Exception& Exception::operator = (const Exception& ex) noexcept
{
	*impl = *ex.impl;
	return *this;
}


std::string Exception::getBacktrace()
{
	std::string backtrace = "Backtrace:\n";
	int index = 1;
	for (std::list<std::string>::iterator it = impl->m_errors.begin(); it != impl->m_errors.end(); ++it)
	{
		backtrace += int_to_str(index) + ". " + (*it) + "\n";
		++index;
	}
	return backtrace;
}

void Exception::appendError(const std::string &error_message)
{
	impl->m_errors.push_back(error_message);
}

std::list<std::string>::iterator Exception::getErrorIterator() const
{
	return impl->m_errors.begin();
}

size_t Exception::getErrorCount() const
{
	return impl->m_errors.size();
}

namespace
{

std::string complex_message(const std::string& message, const std::exception& nested)
{
	return message + " with nested " +
		boost::algorithm::erase_all_copy(
			boost::core::demangle(typeid(nested).name()),
			"class ") + // class prefix is added on MSVC
		" " + nested.what();
}

} // anonymous namespace

LogicError::LogicError(const std::string& message)
	: std::logic_error(message)
{}

LogicError::LogicError(const std::string& message, const std::exception& nested)
	: std::logic_error(complex_message(message, nested))
{}

RuntimeError::RuntimeError(const std::string& message)
	: std::runtime_error(message)
{}

RuntimeError::RuntimeError(const std::string& message, const std::exception& nested)
	: std::runtime_error(complex_message(message, nested))
{}

} // namespace docwire
