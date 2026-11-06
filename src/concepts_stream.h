/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONCEPTS_STREAM_H
#define DF0_CONCEPTS_STREAM_H

#include <concepts>
#include <iosfwd>

namespace df0
{

/**
 * @brief Concept for types that are streamable to `std::ostream`.
 */
template <typename T>
concept streamable = requires (std::ostream& os, const T& value) { { os << value } -> std::convertible_to<std::ostream&>; };

} // namespace df0

#endif // DF0_CONCEPTS_STREAM_H
