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

#ifndef DOCWIRE_LOG_SINK_GLOBALS_H
#define DOCWIRE_LOG_SINK_GLOBALS_H

#include "core_export.h"
#include "log_record.h"

#include <atomic>
#include <functional>
#include <mutex>

namespace docwire::log::detail
{

DOCWIRE_CORE_EXPORT extern std::recursive_mutex g_log_callback_mutex;
DOCWIRE_CORE_EXPORT extern std::function<void(const record&)> g_log_callback;
DOCWIRE_CORE_EXPORT extern std::atomic<bool> g_logging_enabled;

} // namespace docwire::log::detail

#endif // DOCWIRE_LOG_SINK_GLOBALS_H
