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
#include "serialization_base.h"
#include "source_location.h"
#include <functional>
#include <span>
#include <string>
#include <string_view>

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

class DOCWIRE_CORE_EXPORT record
{
public:
	record(source_location location, serialization::array&& context);
	~record();

	record(const record&) = delete;
	record& operator=(const record&) = delete;

	source_location m_location;
	serialization::array m_context;
};

DOCWIRE_CORE_EXPORT void set_filter(const std::string& filter_spec);

/**
 * @brief Gets the current log filter specification string.
 * @return The current filter string.
 */
DOCWIRE_CORE_EXPORT std::string get_filter();

/**
 * @brief Sets the global callback function that will receive all enabled log records.
 * @param callback A function that takes a `const serialization::object&` and processes it.
 *                 This is the primary mechanism for customizing the log sink.
 * @see json_stream_sink
 */
DOCWIRE_CORE_EXPORT void set_sink(std::function<void(const log::record&)> callback);

/**
 * @brief Gets the current log sink function.
 * @return The current sink function.
 */
DOCWIRE_CORE_EXPORT std::function<void(const record&)> get_sink();

/**
 * @brief Creates a base serialization object with common metadata.
 * @param location The source location to be included in the metadata.
 * @return A `serialization::object` containing timestamp, file, line, function, and thread_id.
 */
DOCWIRE_CORE_EXPORT serialization::object create_base_metadata(source_location location);

namespace detail
{
// This is an internal helper function for the log_entry macro.
DOCWIRE_CORE_EXPORT bool is_enabled(const source_location& location, std::span<const std::string_view> entry_tags);
DOCWIRE_CORE_EXPORT bool is_logging_enabled();
}

} // namespace docwire::log

#endif // DOCWIRE_LOG_CORE_H
