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

#ifndef DOCWIRE_LOG_TAGS_H
#define DOCWIRE_LOG_TAGS_H

#include "core_export.h"
#include <string_view>

namespace docwire::log
{

/**
 * @brief Tag for high-level, production-worthy operational events.
 *
 * This tag is intended for use by the final application, not within the SDK itself.
 * The SDK cannot know the application's business context, so it cannot decide what
 * constitutes an "audit" event. Use this tag in your application code to create a
 * clean log channel for production monitoring, separate from the SDK's internal
 * debug logs.
 *
 * Example: `log_entry(log_tags::audit{}, "User logged in", user_id);`
 */
struct DOCWIRE_CORE_EXPORT audit { static constexpr std::string_view string() { return "audit"; } };

/// @brief Tag automatically added to a log entry when a `log_scope` is entered.
struct DOCWIRE_CORE_EXPORT scope_enter { static constexpr std::string_view string() { return "scope_enter"; } };

/// @brief Tag automatically added to a log entry when a `log_scope` is exited.
struct DOCWIRE_CORE_EXPORT scope_exit { static constexpr std::string_view string() { return "scope_exit"; } };

/**
 * @brief Tag automatically added to a log entry created by `log_return`.
 *
 * This allows for easy filtering of all return value logs.
 */
struct DOCWIRE_CORE_EXPORT return_value { static constexpr std::string_view string() { return "return"; } };

} // namespace docwire::log

#endif // DOCWIRE_LOG_TAGS_H
