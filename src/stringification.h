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

#ifndef DOCWIRE_STRINGIFICATION_H
#define DOCWIRE_STRINGIFICATION_H

#include <magic_enum/magic_enum.hpp>
#include <string>
#include <sstream>

namespace docwire
{

template <typename T>
struct stringifier;

template <typename T>
concept to_string_callable = requires { std::to_string(std::declval<T>()); };

template <to_string_callable T>
struct stringifier<T>
{
	std::string operator()(const T& value) { return std::to_string(value); }
};

template <typename T>
concept string_method_equipped = requires { std::declval<T>().string(); };

template <string_method_equipped T>
struct stringifier<T>
{
	std::string operator()(const T& value) { return value.string(); }
};

template <typename T>
concept enum_type = magic_enum::is_scoped_enum<T>::value || magic_enum::is_unscoped_enum<T>::value;

template <enum_type T>
requires (!to_string_callable<T>)
struct stringifier<T>
{
	std::string operator()(const T& value) { return std::string{magic_enum::enum_name(value)}; }
};

template <typename T>
concept streamable = requires (std::ostream& os, T value) { os << value; };

template <streamable T>
requires (!to_string_callable<T> && !string_method_equipped<T> && !enum_type<T>)
struct stringifier<T>
{
	std::string operator()(const T& value)
	{
		std::ostringstream s;
		s << value;
		return s.str();
	}
};

template <typename T>
std::string stringify(const T& value) { return stringifier<T>()(value); }

template<>
struct stringifier<const char*>
{
	std::string operator()(const char* value) { return value; }
};

template<>
struct stringifier<std::string>
{
	std::string operator()(const std::string& value) { return value; }
};

} // namespace docwire

#endif
