/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TYPE_NAME_LIST_H
#define DF0_TYPE_NAME_LIST_H

#include "type_name_base.h"
#include <list>

namespace df0::type_name
{

template<typename T, typename Alloc>
struct pretty_impl<std::list<T, Alloc>> {
	std::string operator()() const { return "std::list<" + pretty<T>() + ">"; }
};

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_LIST_H
