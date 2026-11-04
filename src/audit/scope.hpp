/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_AUDIT_SCOPE_H
#define DOCWIRE_AUDIT_SCOPE_H

#include "auditor_ref.hpp"
#include "scope_tags.hpp"
#include "../source_location.h"

#include <type_traits>
#include <utility>

namespace df0::audit
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
        m_auditor.auditor().record(m_location, df0::audit::scope_enter{}, m_context_fn);
    }

    ~scope()
    {
        m_auditor.auditor().record(m_location, df0::audit::scope_exit{}, m_context_fn);
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

} // namespace df0::audit

#endif
