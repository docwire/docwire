/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_HTTPLIB_PATCHED_H
#define DF0_HTTPLIB_PATCHED_H

#if defined(__SANITIZE_THREAD__) || defined(HELGRIND_ENABLED)
    // Fallback to blocking DNS for ThreadSanitizer and Helgrind.
    // getaddrinfo_a/handle_requests causes a known TSan segfault in glibc
    // and breaks Valgrind's stack unwinder on Ubuntu 24.04 (GCC 13).
	#undef CPPHTTPLIB_USE_NON_BLOCKING_GETADDRINFO
#endif
#include <httplib.h> // IWYU pragma: export

#endif //DF0_HTTPLIB_PATCHED_H
