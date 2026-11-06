/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "fuzzy_match.h"

#include <rapidfuzz/fuzz.hpp>

namespace df0::fuzzy_match
{

double ratio(const std::string& a, const std::string& b)
{
    return rapidfuzz::fuzz::ratio(a, b);
}

} // namespace df0::fuzzy_match
