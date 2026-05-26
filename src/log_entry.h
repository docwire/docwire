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

#ifndef DOCWIRE_LOG_ENTRY_H
#define DOCWIRE_LOG_ENTRY_H

#include <array>
#include "concepts_string.h"
#include "diagnostic_context.h"
#include "log_core.h"
#include "log_tags.h"
#include "named.h"
#include <span>
#include <string>
#include <type_traits>
#include <string_view>
#include <tuple>
#include <utility>

#ifdef NDEBUG
    #include <boost/preprocessor/comma_if.hpp>
    #include <boost/preprocessor/seq/for_each_i.hpp>
    #include <boost/preprocessor/variadic/to_seq.hpp>
#endif

#ifdef NDEBUG
    #define DOCWIRE_LOG_GET_TYPE_ELEM(r, data, i, elem) \
        BOOST_PP_COMMA_IF(i) decltype(elem)
    #define DOCWIRE_LOG_GET_TYPES(...) \
        __VA_OPT__(BOOST_PP_SEQ_FOR_EACH_I(DOCWIRE_LOG_GET_TYPE_ELEM, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))
#endif

namespace docwire::log
{

namespace detail
{

template<typename... Args>
constexpr bool has_audit_tag_impl()
{
    return (std::is_same_v<std::decay_t<Args>, log::audit> || ...);
}

template <typename... Args>
constexpr bool should_log_in_release()
{
    #ifdef NDEBUG
        return has_audit_tag_impl<Args...>();
    #else
        return true;
    #endif
}

serialization::value to_log_value(const context_tag auto& tag)
{
    using T = std::decay_t<decltype(tag)>;
    return std::string(T::string());
}

serialization::value to_log_value(const string_like auto& str)
{
    return std::string(str);
}

inline serialization::value to_log_value(const serialization::object& obj)
{
    return obj;
}

template<typename T1, typename T2>
serialization::value to_log_value(const std::pair<T1, T2>& p)
{
    serialization::object context_obj;
    context_obj.v[p.first] = serialization::typed_summary(p.second);
    return context_obj;
}

template<typename T>
serialization::value to_log_value(const docwire::named::value<T>& p)
{
    serialization::object context_obj;
    context_obj.v[std::string{p.name}] = serialization::typed_summary(p.value);
    return context_obj;
}

} // namespace detail

namespace detail
{

/// @brief A `constexpr` variable template to count how many types in a pack are tags.
template <typename... Ts>
constexpr std::size_t count_tags = (context_tag<Ts> + ... + 0);

template <typename... Args>
constexpr auto collect_tags() {
    // Create an array with the exact size calculated at compile time.
    std::array<std::string_view, count_tags<Args...>> tags{};
    std::size_t index = 0;
    // Use a fold expression to iterate through types and populate the array.
    ([&]<typename T>(std::type_identity<T>) {
        if constexpr (context_tag<T>) {
            tags[index++] = T::string();
        }
    }(std::type_identity<Args>{}), ...);
    return tags;
}
} // namespace detail

template<typename... Args>
void entry(source_location location, std::tuple<Args...>&& args_tuple)
{
    constexpr auto tags = detail::collect_tags<Args...>();
    if (detail::is_enabled(location, std::span{tags}))
    {
        serialization::array context_array;
        std::apply([&](const auto&... items) {
            (context_array.v.push_back(detail::to_log_value(items)), ...);
        }, std::move(args_tuple));
        record(location, std::move(context_array));
    }
}

#ifdef NDEBUG
    #define DOCWIRE_LOG_ENTRY(...) \
        do { \
            if constexpr (docwire::log::detail::should_log_in_release<DOCWIRE_LOG_GET_TYPES(__VA_ARGS__)>()) { \
                if (docwire::log::detail::is_logging_enabled()) { \
                    docwire::log::entry(docwire::source_location::current(), std::make_tuple(DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(__VA_ARGS__))); \
                } else { \
                    (void)0; \
                } \
            } \
        } while (false)
#else
    #define DOCWIRE_LOG_ENTRY(...) \
        do { \
            if (docwire::log::detail::is_logging_enabled()) \
                docwire::log::entry(docwire::source_location::current(), std::make_tuple(DOCWIRE_DIAGNOSTIC_CONTEXT_MAKE_TUPLE(__VA_ARGS__))); \
        } while (false)
#endif

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
    #define log_entry(...) DOCWIRE_LOG_ENTRY(__VA_ARGS__)
#endif

} // namespace docwire::log

#endif // DOCWIRE_LOG_ENTRY_H
