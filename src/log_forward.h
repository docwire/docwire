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

#ifndef DOCWIRE_LOG_FORWARD_H
#define DOCWIRE_LOG_FORWARD_H

#include "log_entry.h"

namespace docwire::log
{

template<typename T, typename... Args>
T&& and_forward_value(const char* expr_str, T&& value, source_location location, Args&&... args)
{
	docwire::log::entry(location, std::make_tuple(docwire::diagnostic_context::make_context_item(expr_str, value), std::forward<Args>(args)...));
	return std::forward<T>(value);
}

} // namespace docwire::log

#ifdef NDEBUG
	#define DOCWIRE_LOG_FORWARD(value, ...) \
		[]<typename T>(const auto& loc, T&& val) -> decltype(auto) { \
			if constexpr (docwire::log::detail::should_log_in_release<decltype(docwire::diagnostic_context::make_context_item(#value, val)) __VA_OPT__(,) DOCWIRE_LOG_GET_TYPES(__VA_ARGS__)>()) { \
                if (docwire::log::detail::is_logging_enabled()) { \
				    return docwire::log::and_forward_value(#value, std::forward<T>(val), loc __VA_OPT__(,) __VA_ARGS__); \
                } else { \
                    return std::forward<T>(val); \
                } \
			} else { \
				return std::forward<T>(val); \
			} \
		}(docwire::source_location::current(), (value))
#else
	#define DOCWIRE_LOG_FORWARD(value, ...) \
        []<typename T>(const auto& loc, T&& val) -> decltype(auto) { \
            if (docwire::log::detail::is_logging_enabled()) \
                return docwire::log::and_forward_value(#value, std::forward<T>(val), loc __VA_OPT__(,) __VA_ARGS__); \
            return std::forward<T>(val); \
        }(docwire::source_location::current(), (value))
#endif

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
	#define log_forward(...) DOCWIRE_LOG_FORWARD(__VA_ARGS__)
#endif

#endif // DOCWIRE_LOG_FORWARD_H
