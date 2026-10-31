#ifndef DOCWIRE_AUDIT_SCOPE_H
#define DOCWIRE_AUDIT_SCOPE_H

#include "auditor_ref.hpp"
#include "scope_tags.hpp"
#include "../source_location.h"

#include <type_traits>
#include <utility>

namespace docwire::audit
{

template<typename auditor_type, typename context_fn_type>
class scope
{
    using stored_context_fn_type = std::decay_t<context_fn_type>;

public:
    scope(auditor_ref<auditor_type> auditor, context_fn_type&& context_fn,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_context_fn(std::forward<context_fn_type>(context_fn)),
          m_location(location)
    {
        m_auditor.auditor().record(m_location, docwire::audit::scope_enter{}, m_context_fn);
    }

    ~scope()
    {
        m_auditor.auditor().record(m_location, docwire::audit::scope_exit{}, m_context_fn);
    }

    scope(const scope&) = delete;
    scope& operator=(const scope&) = delete;

private:
    auditor_ref<auditor_type> m_auditor;
    stored_context_fn_type m_context_fn;
    source_location m_location;
};

template<typename context_fn_type>
class scope<null_auditor, context_fn_type>
{
public:
    constexpr scope(auditor_ref<null_auditor>, context_fn_type&&, source_location = source_location::current()) noexcept {}
    ~scope() noexcept {}
};

template<typename auditor_type, typename context_fn_type>
scope(auditor_ref<auditor_type>, context_fn_type&&) -> scope<auditor_type, context_fn_type>;

template<auditor auditor_type, typename context_fn_type>
scope(auditor_type&, context_fn_type&&) -> scope<auditor_type, context_fn_type>;

} // namespace docwire::audit

#endif
