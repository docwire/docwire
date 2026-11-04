/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_AUDIT_CORE_H
#define DOCWIRE_AUDIT_CORE_H

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
