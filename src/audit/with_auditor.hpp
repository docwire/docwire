#ifndef DOCWIRE_AUDIT_WITH_AUDITOR_H
#define DOCWIRE_AUDIT_WITH_AUDITOR_H

#include "auditor_ref.hpp"

#include <type_traits>
#include <utility>

namespace docwire::audit
{

/**
 * @brief Base for components that store an injected auditor.
 *
 * @note When a component class template modeled on this base needs a
 * default-policy convenience constructor, that constructor must be a
 * constrained **member template**, not a non-template constrained member.
 *
 *       @code
 *       template<stateless_auditor stateless_auditor_type = auditor_type>
 *       component()
 *           : component(audit::auditor_ref<stateless_auditor_type>{})
 *       {}
 *       @endcode
 *
 * Non-template constrained members are not reliably SFINAE-friendly across
 * conforming C++20 implementations during explicit class-template
 * instantiation or type-trait evaluation.
 */
template<typename auditor_type>
class with_auditor : private auditor_ref<auditor_type>
{
protected:
    explicit with_auditor(auditor_ref<auditor_type> auditor)
        : auditor_ref<auditor_type>(auditor) {}

    auditor_ref<auditor_type> auditor() noexcept
    {
        return static_cast<auditor_ref<auditor_type>&>(*this);
    }

    auditor_ref<auditor_type> auditor() const noexcept
    {
        return static_cast<const auditor_ref<auditor_type>&>(*this);
    }
};

} // namespace docwire::audit

#endif
