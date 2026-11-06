/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_AUDITOR_REF_H
#define DF0_AUDIT_AUDITOR_REF_H

#include "core.hpp"

#include <concepts>
#include <type_traits>
#include <utility>

namespace df0::audit
{

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

template<typename T>
concept stateless_auditor =
    std::default_initializable<auditor_ref<T>>;

} // namespace df0::audit

#endif
