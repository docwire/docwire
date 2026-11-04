/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "boost_dll_wrapper.h"

#include <boost/dll/runtime_symbol_info.hpp>

namespace df0::detail
{

std::filesystem::path this_line_location_helper(const this_line_location_t& this_line_location_instance)
{
    return boost::dll::symbol_location(this_line_location_instance).lexically_normal().string();
}

std::filesystem::path program_location()
{
    return std::filesystem::path(boost::dll::program_location().string());
}

} // namespace df0::detail
