#ifndef DOCWIRE_AUDIT_DETAIL_TUPLE_TRAITS_HPP
#define DOCWIRE_AUDIT_DETAIL_TUPLE_TRAITS_HPP

#include <tuple>
#include <type_traits>

namespace docwire::audit::detail
{

template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... Args>
struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

} // namespace docwire::audit::detail

#endif
