/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ICONV_WRAPPER_H
#define DF0_ICONV_WRAPPER_H

#include <cstdint>
#include <string>
#include <string_view>
#include "core_export.h"

namespace df0::detail::iconv_wrapper
{

struct state
{
    std::uintptr_t descriptor{}; // opaque iconv_t handle
    std::string from;
    std::string to;
};

DF0_CORE_EXPORT void open(state& st);
DF0_CORE_EXPORT void close(state& st) noexcept;
DF0_CORE_EXPORT std::string convert(state& st, std::string_view input);

} // namespace df0::detail::iconv_wrapper

#endif
