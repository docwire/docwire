/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_DETAIL_H
#define DF0_AUDIT_DETAIL_H

#include "core.hpp"
#include "audit_tag.hpp"
#include "detail/tuple_traits.hpp"

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace df0::audit::detail
{

template <typename... Args>
concept record_args_valid =
    (1 == (std::invocable<std::decay_t<Args>> + ... + 0)) &&
    ((std::invocable<std::decay_t<Args>> || audit_tag<std::decay_t<Args>>) && ...);

template <typename Auditor, typename Result>
void record_context(Auditor& auditor, source_location location, Result&& result)
{
    using result_type = std::decay_t<Result>;

    if constexpr (is_std_tuple_v<result_type>)
    {
        std::apply([&](auto&&... args) {
            auditor.record(location, std::forward<decltype(args)>(args)...);
        }, std::forward<Result>(result));
    }
    else
    {
        auditor.record(location, std::forward<Result>(result));
    }
}

template <typename Result>
auto normalize_context_tuple(Result&& result)
{
    using result_type = std::decay_t<Result>;

    if constexpr (is_std_tuple_v<result_type>)
    {
        return std::forward<Result>(result);
    }
    else
    {
        return std::make_tuple(std::forward<Result>(result));
    }
}

} // namespace df0::audit::detail

#endif
