/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "boost_demangle_wrapper.h"
#include <boost/core/demangle.hpp>

namespace df0::detail
{

std::string demangle_type_name(const char* name)
{
    return boost::core::demangle(name);
}

} // namespace df0::detail
