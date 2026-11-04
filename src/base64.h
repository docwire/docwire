/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_BASE64_H
#define DF0_BASE64_H

#include "base64_export.h"
#include <span>
#include <string>
#include <vector>

namespace df0::base64
{

DF0_BASE64_EXPORT std::string encode(std::span<const std::byte> data);
DF0_BASE64_EXPORT std::vector<std::byte> decode(std::string_view data);

} // namespace df0::base64

#endif //DF0_BASE64_H
