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
