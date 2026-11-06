/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TYPE_NAME_H
#define DF0_TYPE_NAME_H

#include "boost_demangle_wrapper.h"
#include <typeindex>
#include <string>

// IWYU pragma: begin_exports
#include "type_name_base.h"
#include "type_name_list.h"
#include "type_name_map.h"
#include "type_name_pair.h"
#include "type_name_vector.h"
// IWYU pragma: end_exports

namespace df0::type_name
{

inline std::string normalize_name(const std::string& name)
{
    auto replace_all = [](std::string& s, const std::string& from, const std::string& to) {
        std::size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos) {
            s.replace(pos, from.length(), to);
            pos += to.length();
        }
    };
    std::string normalized = name;
    replace_all(normalized, "__cdecl ", "");
    replace_all(normalized, "virtual ", "");
    replace_all(normalized, "class ", "");
    replace_all(normalized, "struct ", "");
    replace_all(normalized, "::__cxx11", "");
    replace_all(normalized, "std::__1::", "std::");
    replace_all(normalized, "std::__fs::", "std::");
    replace_all(normalized, "(void)", "()");
    replace_all(normalized, " [", "[");
    replace_all(normalized, ", ", ",");
    replace_all(normalized, " >", ">");
    replace_all(normalized, "std::basic_string<char,std::char_traits<char>,std::allocator<char>>", "std::string");
    return normalized;
}

inline std::string from_type_index(std::type_index t)
{
    return normalize_name(detail::demangle_type_name(t.name()));
}

inline std::string pretty_function(const std::string& function_name)
{
    return normalize_name(function_name);
}

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_H
