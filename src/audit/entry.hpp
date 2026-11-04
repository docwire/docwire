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

#ifndef DF0_AUDIT_ENTRY_H
#define DF0_AUDIT_ENTRY_H

#include "auditor_ref.hpp"
#include "tags.hpp"
#include "../source_location.h"

#include <concepts>
#include <tuple>
#include <utility>

namespace df0::audit
{

template<typename auditor_type, std::invocable<> context_fn_type>
void entry(auditor_ref<auditor_type> auditor,
           context_fn_type&& context_fn,
           source_location location = source_location::current())
{
    auditor.auditor().record(location, std::forward<context_fn_type>(context_fn));
}

template<auditor auditor_type, std::invocable<> context_fn_type>
void entry(auditor_type& auditor,
           context_fn_type&& context_fn,
           source_location location = source_location::current())
{
    entry(auditor_ref<auditor_type>(auditor), std::forward<context_fn_type>(context_fn), location);
}

template<typename auditor_type, std::invocable<> context_fn_type, audit_tag TagType>
void entry(auditor_ref<auditor_type> auditor,
           context_fn_type&& context_fn,
           TagType tag,
           source_location location = source_location::current())
{
    auditor.auditor().record(location, tag, std::forward<context_fn_type>(context_fn));
}

template<auditor auditor_type, std::invocable<> context_fn_type, audit_tag TagType>
void entry(auditor_type& auditor,
           context_fn_type&& context_fn,
           TagType tag,
           source_location location = source_location::current())
{
    entry(auditor_ref<auditor_type>(auditor), std::forward<context_fn_type>(context_fn), tag, location);
}

template<typename auditor_type, std::invocable<> context_fn_type, audit_tag... TagTypes>
void entry(auditor_ref<auditor_type> auditor,
           context_fn_type&& context_fn,
           tags<TagTypes...> tags_arg,
           source_location location = source_location::current())
{
    std::apply([&](auto... tag_vals) {
        auditor.auditor().record(location, tag_vals..., std::forward<context_fn_type>(context_fn));
    }, tags<TagTypes...>::as_tuple());
}

template<auditor auditor_type, std::invocable<> context_fn_type, audit_tag... TagTypes>
void entry(auditor_type& auditor,
           context_fn_type&& context_fn,
           tags<TagTypes...> tags_arg,
           source_location location = source_location::current())
{
    entry(auditor_ref<auditor_type>(auditor), std::forward<context_fn_type>(context_fn),
          tags_arg, location);
}

} // namespace df0::audit

#endif
