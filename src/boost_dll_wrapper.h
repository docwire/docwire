/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_BOOST_DLL_WRAPPER_H
#define DF0_BOOST_DLL_WRAPPER_H

#include "core_export.h"

#include <filesystem>

namespace df0::detail
{

using this_line_location_t = std::filesystem::path();

DF0_CORE_EXPORT std::filesystem::path this_line_location_helper(
    const this_line_location_t& this_line_location_instance);

DF0_CORE_EXPORT std::filesystem::path program_location();

} // namespace df0::detail

#endif // DF0_BOOST_DLL_WRAPPER_H
