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

#ifndef DOCWIRE_TYPE_NAME_BASE_H
#define DOCWIRE_TYPE_NAME_BASE_H

#include "core_export.h"
#include <string>
#include <type_traits>
#include <typeindex>

namespace docwire::type_name
{

DOCWIRE_CORE_EXPORT std::string from_type_index(std::type_index t);

template<typename T>
struct pretty_impl {
	std::string operator()() const {
		return from_type_index(typeid(T));
	}
};

template<>
struct pretty_impl<std::string> {
	std::string operator()() const { return "std::string"; }
};

template<typename T>
std::string pretty();

template<typename T>
struct pretty_impl<const T> {
	std::string operator()() const {
		return "const " + pretty<T>();
	}
};

template<typename T>
struct pretty_impl<T*> {
	std::string operator()() const {
		return pretty<T>() + "*";
	}
};

template<typename T>
struct pretty_impl<T&> {
	std::string operator()() const {
		return pretty<std::remove_reference_t<T>>() + "&";
	}
};

template<typename T>
struct pretty_impl<T&&> {
	std::string operator()() const {
		return pretty<std::remove_reference_t<T>>() + "&&";
	}
};

template<typename T>
struct pretty_impl<const T&> {
	std::string operator()() const {
		return pretty<std::add_const_t<std::remove_reference_t<T>>>() + "&";
	}
};

template<typename T>
inline std::string pretty() { return pretty_impl<T>{}(); }

DOCWIRE_CORE_EXPORT std::string pretty_function(const std::string& function_name);

} // namespace docwire::type_name

#endif // DOCWIRE_TYPE_NAME_BASE_H
