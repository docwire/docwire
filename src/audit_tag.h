#ifndef DOCWIRE_AUDIT_TAG_H
#define DOCWIRE_AUDIT_TAG_H

#include "diagnostic_context.h"

#include <tuple>

namespace docwire::audit
{

template<typename T>
concept audit_tag = docwire::context_tag<T>;

template<audit_tag... TagTypes>
struct tags
{
    explicit constexpr tags(TagTypes...) noexcept {}
    static constexpr std::tuple<TagTypes...> as_tuple() noexcept { return {}; }
};

template<audit_tag... TagTypes>
tags(TagTypes...) -> tags<TagTypes...>;

} // namespace docwire::audit

#endif
