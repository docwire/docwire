/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AUDIT_AUDIT_TAG_H
#define DF0_AUDIT_AUDIT_TAG_H

#include <concepts>
#include <string_view>

namespace df0::audit
{

/**
 * @brief Concept for types that behave as audit tags.
 *
 * Audit tags identify the semantic category of an audit event in a
 * structured, type-safe way.
 */
template<typename T>
concept audit_tag = requires
{
    { T::string() } -> std::convertible_to<std::string_view>;
};

} // namespace df0::audit

#endif
