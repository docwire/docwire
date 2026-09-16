#ifndef DOCWIRE_AUDIT_REF_H
#define DOCWIRE_AUDIT_REF_H

#include "audit_core.h"

#include <type_traits>
#include <utility>

namespace docwire::audit
{

template<typename auditor_type>
class auditor_ref
{
public:
    constexpr auditor_ref(auditor_type& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr auditor_type& auditor() noexcept { return *m_ptr; }
    constexpr const auditor_type& auditor() const noexcept { return *m_ptr; }

private:
    auditor_type* m_ptr;
};

template<typename auditor_type>
requires std::is_empty_v<auditor_type>
class auditor_ref<auditor_type> : public auditor_type
{
public:
    constexpr auditor_ref() requires std::default_initializable<auditor_type> = default;
    constexpr auditor_ref(auditor_type&) noexcept {}

    constexpr auditor_type& auditor() noexcept { return *this; }
    constexpr const auditor_type& auditor() const noexcept { return *this; }
};

template<typename auditor_type>
class with_auditor : private auditor_ref<auditor_type>
{
protected:
    with_auditor() = default;
    explicit with_auditor(auditor_ref<auditor_type> auditor)
        : auditor_ref<auditor_type>(auditor) {}

    auditor_type& auditor() noexcept { return auditor_ref<auditor_type>::auditor(); }
    const auditor_type& auditor() const noexcept { return auditor_ref<auditor_type>::auditor(); }
};

} // namespace docwire::audit

#endif
