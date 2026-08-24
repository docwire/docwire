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

#ifndef DOCWIRE_LOG_RECORD_H
#define DOCWIRE_LOG_RECORD_H

#include "core_export.h"
#include "serialization_base.h"
#include "source_location.h"

#include <atomic>
#include <functional>
#include <mutex>

namespace docwire::log
{

/**
 * @brief Immutable log record containing the source location and structured context.
 */
struct record;

namespace detail
{
DOCWIRE_CORE_EXPORT extern std::recursive_mutex g_log_callback_mutex;
DOCWIRE_CORE_EXPORT extern std::function<void(const record&)> g_log_callback;
DOCWIRE_CORE_EXPORT extern std::atomic<bool> g_logging_enabled;
}

struct record
{
    source_location m_location;
    serialization::array m_context;

    record(source_location location, serialization::array&& context)
        : m_location(location), m_context(std::move(context))
    {
    }

    ~record()
    {
        try
        {
            std::lock_guard lock(detail::g_log_callback_mutex);
            if (detail::g_logging_enabled.load(std::memory_order_acquire) && detail::g_log_callback)
                detail::g_log_callback(*this);
        }
        catch (...)
        {
            // Destructors must never throw.
        }
    }
};

} // namespace docwire::log

#endif // DOCWIRE_LOG_RECORD_H
