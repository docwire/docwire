/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_STRONG_TYPE_H
#define DF0_STRONG_TYPE_H

#include <utility>
namespace df0
{
template <typename T, typename Tag> class strong_type
{
  public:
    using value_type = T;

    explicit constexpr strong_type(T v) noexcept : value_(std::move(v)) {}

    constexpr T get() const noexcept { return value_; }

  private:
    T value_;
};

} // namespace df0

#endif
