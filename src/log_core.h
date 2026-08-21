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

#ifndef DOCWIRE_LOG_CORE_H
#define DOCWIRE_LOG_CORE_H

#include "core_export.h"
#include "log_record.h"
#include "log_filter_rules.h"
#include "log_filter_globals.h"
#include "log_sink_globals.h"
#include "serialization_base.h"
#include "source_location.h"
#include "type_name_base.h"
#include "serialization_filesystem.h" // IWYU pragma: keep
#include "serialization_thread_id.h"   // IWYU pragma: keep

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <mutex>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>

#include "boost_datetime_wrapper.h"

/**
 * @brief Provides a modern, high-performance, and structured logging framework.
 *
 * The `docwire::log` namespace contains a powerful logging system designed for both
 * deep debugging and zero-overhead production use. It features:
 *
 * - **Structured JSON Output**: All log records are generated as structured JSON objects,
 *   making them easy to parse, query, and integrate with modern log analysis platforms.
 *
 * - **Zero-Cost in Release Builds**: By default, most logging calls (`log_entry`, `log_scope`)
 *   are completely compiled out in release builds (`NDEBUG` is defined), ensuring they have
 *   zero performance impact on your production code. Only logs explicitly marked with a
 *   persistent tag (like `log::audit`) are retained.
 *
 * - **Sink and Filter Model**: The framework is silent by default. To receive logs, you must
 *   programmatically set a "sink" (a callback function that receives log records) and a
 *   "filter" (a string that specifies which logs to enable).
 *
 * - **Powerful Filtering**: Filter logs based on source file, function name, or custom tags
 *   using a simple wildcard-based syntax.
 */

namespace docwire::log
{

inline void set_filter(const std::string& filter_spec)
{
    std::lock_guard lock(detail::g_log_filter_mutex);
    detail::g_log_filter = detail::parse_log_filter(filter_spec);
    detail::g_log_filter_str = filter_spec;
}

/**
 * @brief Gets the current log filter specification string.
 * @return The current filter string.
 */
inline std::string get_filter()
{
    std::lock_guard lock(detail::g_log_filter_mutex);
    return detail::g_log_filter_str;
}

/**
 * @brief Sets the global callback function that will receive all enabled log records.
 * @param callback A function that takes a `const serialization::object&` and processes it.
 *                 This is the primary mechanism for customizing the log sink.
 * @see json_stream_sink
 */
inline void set_sink(std::function<void(const log::record&)> callback)
{
    std::lock_guard lock(detail::g_log_callback_mutex);
    detail::g_log_callback = std::move(callback);
    detail::g_logging_enabled.store(static_cast<bool>(detail::g_log_callback), std::memory_order_release);
}

/**
 * @brief Gets the current log sink function.
 * @return The current sink function.
 */
inline std::function<void(const record&)> get_sink()
{
    std::lock_guard lock(detail::g_log_callback_mutex);
    return detail::g_log_callback;
}

/**
 * @brief Creates a base serialization object with common metadata.
 * @param location The source location to be included in the metadata.
 * @return A `serialization::object` containing timestamp, file, line, function, and thread_id.
 */
inline serialization::object create_base_metadata(source_location location)
{
    serialization::object metadata;
    std::string timestamp = docwire::detail::local_iso_datetime_with_timezone_offset();

    metadata.v = {
        {"timestamp", timestamp},
        {"file", serialization::full(std::filesystem::path(location.file_name()).filename())},
        {"line", static_cast<std::int64_t>(location.line())},
        {"function", docwire::type_name::pretty_function(location.function_name())},
        {"thread_id", serialization::full(std::this_thread::get_id())}
    };
    return metadata;
}

namespace detail
{
// This is an internal helper function for the log_entry macro.
inline bool is_logging_enabled()
{
    return g_logging_enabled.load(std::memory_order_acquire);
}

inline bool is_enabled(const source_location& location, std::span<const std::string_view> entry_tags)
{
    std::lock_guard lock(g_log_filter_mutex);
    const auto& filter = g_log_filter;

    std::string filename = std::filesystem::path(location.file_name()).filename().string();
    std::string funcname = docwire::type_name::pretty_function(location.function_name());

    // 1. Process "deny" rules first. A single negative match immediately disables the log.
    for (const auto& rule : filter.rules)
    {
        if (!rule.is_negative)
            continue;

        if ((rule.type == filter_rule::FILE && wildcard_match(rule.value, filename)) ||
            (rule.type == filter_rule::FUNC && wildcard_match(rule.value, funcname)) ||
            (rule.type == filter_rule::TAG && std::any_of(entry_tags.begin(), entry_tags.end(), [&](auto tag) { return wildcard_match(rule.value, tag); })))
        {
            return false;
        }
    }

    // 2. If the global wildcard '*' is enabled, any log that has not been explicitly
    // denied by this point is allowed. This is a fast path.
    if (filter.wildcard_enabled)
        return true;

    // 3. Process all positive rules. The first match of any type is sufficient to enable the log.
    for (const auto& rule : filter.rules)
    {
        if (rule.is_negative)
            continue;

        if ((rule.type == filter_rule::FILE && wildcard_match(rule.value, filename)) ||
            (rule.type == filter_rule::FUNC && wildcard_match(rule.value, funcname)) ||
            (rule.type == filter_rule::TAG && std::any_of(entry_tags.begin(), entry_tags.end(), [&](auto tag) { return wildcard_match(rule.value, tag); })))
        {
            return true;
        }
    }

    return false; // Not enabled by any rule.
}
} // namespace detail

} // namespace docwire::log

#endif // DOCWIRE_LOG_CORE_H
