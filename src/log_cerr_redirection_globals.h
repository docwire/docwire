#ifndef DOCWIRE_LOG_CERR_REDIRECTION_GLOBALS_H
#define DOCWIRE_LOG_CERR_REDIRECTION_GLOBALS_H

#include "core_export.h"

#include <mutex>

namespace docwire::log::detail
{

DOCWIRE_CORE_EXPORT extern std::mutex cerr_redirection_mutex;

} // namespace docwire::log::detail

#endif
