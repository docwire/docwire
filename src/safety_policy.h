/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SAFETY_POLICY_H
#define DF0_SAFETY_POLICY_H

namespace df0
{

/**
 * @brief Defines the safety policy for operations.
 */
enum class safety_policy { 
    /// Perform runtime checks and throw exceptions on violations.
    strict, 
    /// Skip runtime checks for performance; undefined behavior on violations.
    relaxed 
};
using enum safety_policy;
constexpr inline safety_policy default_safety_level = strict;

}

#endif // DF0_SAFETY_POLICY_H
