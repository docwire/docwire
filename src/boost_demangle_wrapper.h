/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_BOOST_DEMANGLE_WRAPPER_H
#define DF0_BOOST_DEMANGLE_WRAPPER_H

#include "core_export.h"
#include <string>

namespace df0::detail
{
DF0_CORE_EXPORT std::string demangle_type_name(const char* name);
}

#endif // DF0_BOOST_DEMANGLE_WRAPPER_H
