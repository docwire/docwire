/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_HTTPLIB_PATCHED_H
#define DOCWIRE_HTTPLIB_PATCHED_H

#if defined(__SANITIZE_THREAD__) || defined(HELGRIND_ENABLED)
    // Fallback to blocking DNS for ThreadSanitizer and Helgrind.
    // getaddrinfo_a/handle_requests causes a known TSan segfault in glibc
    // and breaks Valgrind's stack unwinder on Ubuntu 24.04 (GCC 13).
	#undef CPPHTTPLIB_USE_NON_BLOCKING_GETADDRINFO
#endif
#include <httplib.h> // IWYU pragma: export

#endif //DOCWIRE_HTTPLIB_PATCHED_H
