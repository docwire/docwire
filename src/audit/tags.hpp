#ifndef DOCWIRE_AUDIT_TAGS_H
#define DOCWIRE_AUDIT_TAGS_H

#include "audit_tag.hpp"

#include <tuple>

namespace docwire::audit
{

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
