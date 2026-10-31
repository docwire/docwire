#ifndef DOCWIRE_AUDIT_ENTRY_H
#define DOCWIRE_AUDIT_ENTRY_H

#include "auditor_ref.hpp"
#include "tags.hpp"
#include "../source_location.h"

#include <concepts>
#include <tuple>
#include <utility>

namespace docwire::audit
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

} // namespace docwire::audit

#endif
