/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TYPE_NAME_PAIR_H
#define DF0_TYPE_NAME_PAIR_H

#include "type_name_base.h"
#include <utility>

namespace df0::type_name
{

template<typename T1, typename T2>
struct pretty_impl<std::pair<T1, T2>> {
	std::string operator()() const { return "std::pair<" + pretty<T1>() + "," + pretty<T2>() + ">"; }
};

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_PAIR_H
