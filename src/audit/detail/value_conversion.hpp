#ifndef DOCWIRE_AUDIT_DETAIL_VALUE_CONVERSION_HPP
#define DOCWIRE_AUDIT_DETAIL_VALUE_CONVERSION_HPP

#include "audit/audit_tag.hpp"
#include "boost_datetime_wrapper.h"
#include "concepts_string.h"
#include "named.h"
#include "serialization_base.h"
#include "serialization_filesystem.h"
#include "serialization_thread_id.h"
#include "source_location.h"
#include "type_name_base.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

namespace docwire::audit::detail
{

serialization::value to_log_value(const audit_tag auto& tag)
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

template<typename T>
serialization::value to_log_value(const T& value)
{
    return serialization::typed_summary(value);
}

template <typename... Ts>
constexpr std::size_t count_tags = (audit_tag<Ts> + ... + 0);

template <typename... Args>
constexpr auto collect_tags()
{
    std::array<std::string_view, count_tags<Args...>> tags{};
    std::size_t index = 0;

    ([&]<typename T>(std::type_identity<T>)
    {
        if constexpr (audit_tag<T>)
        {
            tags[index++] = T::string();
        }
    }(std::type_identity<Args>{}), ...);

    return tags;
}

template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... Args>
struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

template <typename... Args>
serialization::array build_context(Args&&... args)
{
    serialization::array context;

    auto append = [&](auto&& item)
    {
        context.v.push_back(to_log_value(std::forward<decltype(item)>(item)));
    };

    auto append_result = [&](auto&& result)
    {
        using result_type = std::decay_t<decltype(result)>;

        if constexpr (is_std_tuple_v<result_type>)
        {
            std::apply([&](auto&&... tuple_items)
            {
                (append(std::forward<decltype(tuple_items)>(tuple_items)), ...);
            }, std::forward<decltype(result)>(result));
        }
        else
        {
            append(std::forward<decltype(result)>(result));
        }
    };

    auto append_arg = [&](auto&& arg)
    {
        using arg_type = std::decay_t<decltype(arg)>;

        if constexpr (std::invocable<arg_type>)
        {
            append_result(std::invoke(std::forward<decltype(arg)>(arg)));
        }
        else
        {
            append(std::forward<decltype(arg)>(arg));
        }
    };

    (append_arg(std::forward<Args>(args)), ...);

    return context;
}

inline serialization::object create_record_object(
    source_location location,
    serialization::array context)
{
    serialization::object record_object;

    std::string timestamp =
        docwire::detail::local_iso_datetime_with_timezone_offset();

    record_object.v = {
        {"timestamp", timestamp},
        {"file", serialization::full(std::filesystem::path(location.file_name()).filename())},
        {"line", static_cast<std::int64_t>(location.line())},
        {"function", docwire::type_name::pretty_function(location.function_name())},
        {"thread_id", serialization::full(std::this_thread::get_id())},
        {"log", std::move(context)}
    };

    return record_object;
}

} // namespace docwire::audit::detail

#endif
