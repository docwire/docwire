#ifndef DOCWIRE_AUDIT_SCOPE_TAGS_H
#define DOCWIRE_AUDIT_SCOPE_TAGS_H

#include <string_view>

namespace docwire::audit
{

struct scope_enter
{
    static constexpr std::string_view string() { return "scope_enter"; }
};

struct scope_exit
{
    static constexpr std::string_view string() { return "scope_exit"; }
};

} // namespace docwire::audit

#endif
