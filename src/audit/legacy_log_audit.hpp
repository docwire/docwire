#ifndef DOCWIRE_AUDIT_LEGACY_LOG_AUDIT_H
#define DOCWIRE_AUDIT_LEGACY_LOG_AUDIT_H

#include "audit.hpp"
#include "log_entry.h"

namespace docwire::audit
{

struct legacy_log_audit
{
    template<typename... Args>
    void record(source_location location, Args&&... args)
    {
        constexpr auto tags = docwire::log::detail::collect_tags<Args...>();
        if (docwire::log::detail::is_enabled(location, std::span{tags}))
        {
            serialization::array context;
            (context.v.push_back(docwire::log::detail::to_log_value(args)), ...);
            docwire::log::record(location, std::move(context));
        }
    }
};

} // namespace docwire::audit

#endif
