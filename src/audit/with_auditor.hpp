#ifndef DOCWIRE_AUDIT_WITH_AUDITOR_H
#define DOCWIRE_AUDIT_WITH_AUDITOR_H

#include "auditor_ref.hpp"

#include <type_traits>
#include <utility>

namespace docwire::audit
{

template<typename auditor_type>
class with_auditor : private auditor_ref<auditor_type>
{
protected:
    explicit with_auditor(auditor_ref<auditor_type> auditor)
        : auditor_ref<auditor_type>(auditor) {}

    auditor_type& auditor() noexcept { return auditor_ref<auditor_type>::auditor(); }
    auditor_type& auditor() const noexcept { return auditor_ref<auditor_type>::auditor(); }
};

} // namespace docwire::audit

#endif
