#ifndef DOCWIRE_AUDIT_AUDITOR_REF_H
#define DOCWIRE_AUDIT_AUDITOR_REF_H

#include "core.hpp"

#include <type_traits>
#include <utility>

namespace docwire::audit
{

/**
 * @brief Lightweight non-owning reference to an injected auditor.
 *
 * In the general case this stores only a pointer to the auditor. The
 * empty-auditor specialization is stateless and usually consumes no storage.
 *
 * Intended to be accepted and returned by value. Callers should not wrap it
 * again with `auditor_ref<auditor_type>(...)` after obtaining it from an
 * owning component.
 */
template<typename auditor_type>
class auditor_ref
{
public:
    constexpr auditor_ref(auditor_type& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr auditor_type& auditor() noexcept { return *m_ptr; }
    constexpr auditor_type& auditor() const noexcept { return *m_ptr; }

private:
    auditor_type* m_ptr;
};

/**
 * @brief Empty-auditor specialization of auditor_ref.
 *
 * This specialization has zero data members when the auditor is empty and
 * provides a default constructor only for default-constructible empty
 * auditors.
 */
template<typename auditor_type>
requires std::is_empty_v<auditor_type>
class auditor_ref<auditor_type> : public auditor_type
{
public:
    constexpr auditor_ref() requires std::default_initializable<auditor_type> = default;
    constexpr auditor_ref(auditor_type&) noexcept {}

    constexpr auditor_type& auditor() noexcept { return *this; }
    constexpr auditor_type& auditor() const noexcept
    {
        return const_cast<auditor_type&>(static_cast<const auditor_type&>(*this));
    }
};

} // namespace docwire::audit

#endif
