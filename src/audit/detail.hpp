#ifndef DOCWIRE_AUDIT_DETAIL_H
#define DOCWIRE_AUDIT_DETAIL_H

#include "audit/core.hpp"
#include "audit/audit_tag.hpp"
#include "audit/detail/tuple_traits.hpp"

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace docwire::audit::detail
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

} // namespace docwire::audit::detail

#endif
