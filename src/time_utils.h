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
