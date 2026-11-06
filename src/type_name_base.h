/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TYPE_NAME_BASE_H
#define DF0_TYPE_NAME_BASE_H

#include "core_export.h"
#include <string>
#include <type_traits>
#include <typeindex>

namespace df0::type_name
{

std::string from_type_index(std::type_index t);

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

std::string pretty_function(const std::string& function_name);

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_BASE_H
