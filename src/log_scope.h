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

#ifndef DOCWIRE_LOG_SCOPE_H
#define DOCWIRE_LOG_SCOPE_H

#include "log_entry.h"

namespace docwire::log {

namespace detail {

/**
 * @brief RAII class for logging scope entry and exit.
 */
template <typename... Args>
class scope {
public:
    scope(const Args&... args, const source_location& location = source_location::current()) noexcept
        : m_location(location), m_args_tuple(args...)
    {
        if (detail::is_logging_enabled())
        {
            docwire::log::entry(m_location, std::tuple_cat(std::make_tuple(log::scope_enter{}), m_args_tuple));
        }
    }

    ~scope() noexcept
    {
        if (detail::is_logging_enabled())
        {
            try {
                docwire::log::entry(m_location, std::tuple_cat(std::make_tuple(log::scope_exit{}), m_args_tuple));
            } catch(...) {}
        }
    }

private:
    source_location m_location;
    std::tuple<Args...> m_args_tuple;
};

// An empty struct to be used when the log scope should be completely compiled out.
/**
 * @brief A no-op scope class used when logging is disabled or in release builds.
 */
struct empty_scope {
    // A constructor that accepts any arguments and does nothing.
    // This is designed to be a 'sink' for any arguments passed from the log_scope macro
    // when logging is disabled for a release build, ensuring compilation succeeds
    // without generating any code.
    template <typename... T>
    [[maybe_unused]] explicit empty_scope(T&&...) noexcept
    {}
};

#ifdef NDEBUG
/// Constant indicating if the build is debug.
constexpr bool is_debug_build = false;
#else
/// Constant indicating if the build is debug.
constexpr bool is_debug_build = true;
#endif

} // namespace detail

// The public-facing `scope` class template.
// It inherits from the real implementation or an empty struct based on build mode.
/**
 * @brief Represents a logging scope.
 * 
 * In debug builds (or if logging is enabled), this logs entry and exit of the scope.
 * In release builds, it may compile to a no-op depending on configuration.
 */
template <typename... Args>
class scope : public std::conditional_t< // Note: This is now the public `scope`
    detail::is_debug_build || detail::should_log_in_release<Args...>(),
    detail::scope<Args...>,
    detail::empty_scope
>
{
public:
    using base = std::conditional_t<detail::is_debug_build || detail::should_log_in_release<Args...>(), detail::scope<Args...>, detail::empty_scope>;
    using base::base;
    // Constructor that takes a tuple of arguments and forwards it to the base.
    [[maybe_unused]] explicit scope(const Args&... args, const source_location& location = source_location::current()) noexcept
        : base(args..., location)
    {}
};

// Deduction guide to allow creating a scope object without explicitly specifying template arguments.
template<typename... Args>
scope(const Args&...) -> scope<Args...>;

} // namespace docwire::log

#define DOCWIRE_LOG_SCOPE_CONCAT_IMPL(a, b) a##b
#define DOCWIRE_LOG_SCOPE_CONCAT(a, b) DOCWIRE_LOG_SCOPE_CONCAT_IMPL(a, b)

#define DOCWIRE_LOG_SCOPE(...) \
    [[maybe_unused]] docwire::log::scope \
    DOCWIRE_LOG_SCOPE_CONCAT(docwire_log_scope_object_at_line_, __LINE__) {DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(__VA_ARGS__)}

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
    #define log_scope(...) DOCWIRE_LOG_SCOPE(__VA_ARGS__)
#endif

#endif // DOCWIRE_LOG_SCOPE_H
