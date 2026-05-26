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

#include "type_name.h"
#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>

namespace docwire::type_name
{

namespace
{

/**
 * @brief Cleans up platform-specific artifacts from demangled type names.
 *
 * This function removes common, noisy substrings that different compilers and
 * standard library implementations add to type names, ensuring a more
 * consistent and readable representation across platforms.
 */
std::string normalize_name(const std::string& name)
{
	std::string normalized = name;
	boost::algorithm::erase_all(normalized, "__cdecl "); // MSVC calling convention
	boost::algorithm::erase_all(normalized, "virtual ");
	boost::algorithm::erase_all(normalized, "class ");
	boost::algorithm::erase_all(normalized, "struct ");
	boost::algorithm::replace_all(normalized, "::__cxx11", ""); // libstdc++ (GCC) ABI versioning namespace/tag
	boost::algorithm::replace_all(normalized, "std::__1::", "std::"); // libc++ (Clang) versioning namespace
	boost::algorithm::replace_all(normalized, "std::__fs::", "std::"); // libc++ (Clang) filesystem internal namespace
	boost::algorithm::replace_all(normalized, "(void)", "()");
	boost::algorithm::replace_all(normalized, " [", "[");
	boost::algorithm::replace_all(normalized, ", ", ",");
	boost::algorithm::replace_all(normalized, " >", ">");
	return normalized;
}

} // anonymous namespace

std::string from_type_index(std::type_index t)
{
	return normalize_name(boost::core::demangle(t.name()));
}

std::string pretty_function(const std::string& function_name)
{
	return normalize_name(function_name);
}

} // namespace docwire::type_name
