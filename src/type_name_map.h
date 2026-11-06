/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TYPE_NAME_MAP_H
#define DF0_TYPE_NAME_MAP_H

#include "type_name_base.h"
#include <map>
#include <unordered_map>

namespace df0::type_name
{

template<typename Key, typename T, typename Compare, typename Alloc>
struct pretty_impl<std::map<Key, T, Compare, Alloc>> {
	std::string operator()() const { return "std::map<" + pretty<Key>() + "," + pretty<T>() + ">"; }
};

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
struct pretty_impl<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> {
    std::string operator()() const { return "std::unordered_map<" + pretty<Key>() + "," + pretty<T>() + ">"; }
};

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_MAP_H
