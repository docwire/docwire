/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TIME_UTILS_H
#define DF0_TIME_UTILS_H

#include <ctime>

namespace df0
{

inline tm* thread_safe_gmtime(const time_t* timer, struct tm& time_buffer)
{
#ifdef _WIN32
	(void)time_buffer;
	return gmtime(timer);
#else
	return gmtime_r(timer, &time_buffer);
#endif
}

} // namespace df0

#endif
