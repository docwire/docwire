/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_UNIQUE_IDENTIFIER_GLOBALS_H
#define DF0_UNIQUE_IDENTIFIER_GLOBALS_H

#include "core_export.h"
#include <cstddef>

namespace df0::detail
{

/**
 * @brief Returns the next process-wide unique identifier value.
 *
 * @return A monotonically increasing size_t value.
 *
 * @note This function is implemented in df0_core and exported from it.
 *       Keeping the counter here ensures identifiers remain unique across
 *       DLL boundaries.
 */
DF0_CORE_EXPORT size_t next_unique_identifier();

} // namespace df0::detail

#endif // DF0_UNIQUE_IDENTIFIER_GLOBALS_H
