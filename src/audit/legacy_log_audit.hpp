#ifndef DOCWIRE_AUDIT_LEGACY_LOG_AUDIT_H
#define DOCWIRE_AUDIT_LEGACY_LOG_AUDIT_H

#include "audit.hpp"
#include "log_entry.h"

namespace docwire::audit
{

struct legacy_log_audit
{
    template<typename... Args>
    requires audit::detail::record_args_valid<Args...>
    void record(source_location location, Args&&... args)
    {
        constexpr auto tags = docwire::log::detail::collect_tags<Args...>();
        if (!docwire::log::detail::is_enabled(location, std::span{tags}))
            return;

        serialization::array context;

        auto append_result = [&context](auto&& result)
        {
            using result_type = std::decay_t<decltype(result)>;

            if constexpr (audit::detail::is_std_tuple_v<result_type>)
            {
                std::apply([&context](auto&&... item) {
                    (context.v.push_back(docwire::log::detail::to_log_value(std::forward<decltype(item)>(item))), ...);
                }, std::forward<decltype(result)>(result));
            }
            else
            {
                context.v.push_back(docwire::log::detail::to_log_value(std::forward<decltype(result)>(result)));
            }
        };

        auto append_arg = [&](auto&& arg)
        {
            using arg_type = std::decay_t<decltype(arg)>;

            if constexpr (std::invocable<arg_type>)
            {
                append_result(std::invoke(std::forward<decltype(arg)>(arg)));
            }
            else
            {
                context.v.push_back(docwire::log::detail::to_log_value(std::forward<decltype(arg)>(arg)));
            }
        };

        (append_arg(std::forward<Args>(args)), ...);
        docwire::log::record(location, std::move(context));
    }
};

} // namespace docwire::audit

#endif
