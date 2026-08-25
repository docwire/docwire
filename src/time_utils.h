/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_TIME_UTILS_H
#define DOCWIRE_TIME_UTILS_H

#include <ctime>
#include "log_scope.h"

namespace docwire
{

inline tm* thread_safe_gmtime(const time_t* timer, struct tm& time_buffer)
{
	DOCWIRE_LOG_SCOPE();
#ifdef _WIN32
	(void)time_buffer;
	return gmtime(timer);
#else
	return gmtime_r(timer, &time_buffer);
#endif
}

} // namespace docwire

#endif
