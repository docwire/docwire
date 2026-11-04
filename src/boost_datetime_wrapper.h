/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_BOOST_DATETIME_WRAPPER_H
#define DF0_BOOST_DATETIME_WRAPPER_H

#include "core_export.h"

#include <string>

namespace df0::detail
{

/**
 * @brief Returns the current local time as an ISO extended string with local timezone offset.
 *
 * @return A timestamp in the format:
 *         `YYYY-MM-DDTHH:MM:SS+HHMM` or `YYYY-MM-DDTHH:MM:SS-HHMM`.
 */
DF0_CORE_EXPORT std::string local_iso_datetime_with_timezone_offset();

} // namespace df0::detail

#endif // DF0_BOOST_DATETIME_WRAPPER_H
