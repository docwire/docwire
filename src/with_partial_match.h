/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_WITH_PARTIAL_MATCH_H
#define DF0_WITH_PARTIAL_MATCH_H

#include <string_view>

namespace with
{

/**
 * @brief A wrapper to indicate that partial matching is allowed during conversion.
 */
struct partial_match
{
    std::string_view v;
    operator std::string_view() const noexcept { return v; }
};

} // namespace with

#endif // DF0_WITH_PARTIAL_MATCH_H
