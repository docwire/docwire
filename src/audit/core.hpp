/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_CORE_H
#define DF0_AUDIT_CORE_H

#include "../source_location.h"

#include <concepts>
#include <tuple>

namespace df0::audit
{

struct null_auditor
{
    template<typename... Args>
    constexpr void record(source_location, Args&&...) const noexcept {}
};

/// @brief A no-op context provider for audit entries and scopes that carry no structured context.
/// @note Using this guarantees that no context is evaluated or stored for the audit expression.
inline constexpr auto no_context = []() noexcept -> std::tuple<> { return {}; };

template<typename A>
concept auditor = requires(A& a, source_location loc)
{
    { a.record(loc, no_context) } -> std::same_as<void>;
};

} // namespace df0::audit

#endif
