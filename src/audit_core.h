#ifndef DOCWIRE_AUDIT_CORE_H
#define DOCWIRE_AUDIT_CORE_H

#include "source_location.h"

#include <concepts>
#include <tuple>

namespace docwire::audit
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

} // namespace docwire::audit

#endif
