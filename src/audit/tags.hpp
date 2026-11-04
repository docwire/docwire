/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_TAGS_H
#define DF0_AUDIT_TAGS_H

#include "audit_tag.hpp"

#include <tuple>

namespace df0::audit
{

template<audit_tag... TagTypes>
struct tags
{
    explicit constexpr tags(TagTypes...) noexcept {}
    static constexpr std::tuple<TagTypes...> as_tuple() noexcept { return {}; }
};

template<audit_tag... TagTypes>
tags(TagTypes...) -> tags<TagTypes...>;

} // namespace df0::audit

#endif
