#ifndef DOCWIRE_AUDIT_H
#define DOCWIRE_AUDIT_H

#include "diagnostic_context.h"
#include "log_tags.h"
#include "source_location.h"

#include <concepts>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace docwire::audit
{

struct null_auditor
{
    template<typename... Args>
    constexpr void record(source_location, Args&&...) const noexcept {}
};

template<typename A>
concept auditor = requires(A& a, source_location loc)
{
    { a.record(loc) } -> std::same_as<void>;
};

template<typename T>
concept audit_tag = docwire::context_tag<T>;

namespace detail
{

template <typename... Args>
concept record_args_valid =
    (1 == (std::invocable<std::decay_t<Args>> + ... + 0)) &&
    ((std::invocable<std::decay_t<Args>> || audit_tag<std::decay_t<Args>>) && ...);

template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... Args>
struct is_std_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

template <typename Auditor, typename Result>
void record_context(Auditor& auditor, source_location location, Result&& result)
{
    using result_type = std::decay_t<Result>;

    if constexpr (is_std_tuple_v<result_type>)
    {
        std::apply([&](auto&&... args) {
            auditor.record(location, std::forward<decltype(args)>(args)...);
        }, std::forward<Result>(result));
    }
    else
    {
        auditor.record(location, std::forward<Result>(result));
    }
}

template <typename Result>
auto normalize_context_tuple(Result&& result)
{
    using result_type = std::decay_t<Result>;

    if constexpr (is_std_tuple_v<result_type>)
    {
        return std::forward<Result>(result);
    }
    else
    {
        return std::make_tuple(std::forward<Result>(result));
    }
}

} // namespace detail

/// @brief A no-op context provider for audit entries and scopes that carry no structured context.
/// @note Using this guarantees that no context is evaluated or stored for the audit expression.
inline constexpr auto no_context = []() noexcept -> std::tuple<> { return {}; };

template<typename auditor_type>
class auditor_ref
{
public:
    constexpr auditor_ref(auditor_type& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr auditor_type& auditor() noexcept { return *m_ptr; }
    constexpr const auditor_type& auditor() const noexcept { return *m_ptr; }

private:
    auditor_type* m_ptr;
};

template<typename auditor_type>
requires std::is_empty_v<auditor_type>
class auditor_ref<auditor_type> : public auditor_type
{
public:
    constexpr auditor_ref() requires std::default_initializable<auditor_type> = default;
    constexpr auditor_ref(auditor_type&) noexcept {}

    constexpr auditor_type& auditor() noexcept { return *this; }
    constexpr const auditor_type& auditor() const noexcept { return *this; }
};

template<typename auditor_type>
class with_auditor : private auditor_ref<auditor_type>
{
protected:
    with_auditor() = default;
    explicit with_auditor(auditor_ref<auditor_type> auditor)
        : auditor_ref<auditor_type>(auditor) {}

    auditor_type& auditor() noexcept { return auditor_ref<auditor_type>::auditor(); }
    const auditor_type& auditor() const noexcept { return auditor_ref<auditor_type>::auditor(); }
};

template<typename auditor_type, audit_tag... tag_type, std::invocable<> context_fn_type>
void entry(auditor_ref<auditor_type> auditor,
           tag_type... tags,
           context_fn_type&& context_fn,
           source_location location = source_location::current())
{
    auditor.auditor().record(location, tags..., std::forward<context_fn_type>(context_fn));
}

template<auditor auditor_type, audit_tag... tag_type, std::invocable<> context_fn_type>
void entry(auditor_type& auditor,
           tag_type... tags,
           context_fn_type&& context_fn,
           source_location location = source_location::current())
{
    entry(auditor_ref<auditor_type>(auditor), tags...,
          std::forward<context_fn_type>(context_fn), location);
}

template<typename auditor_type, typename context_fn_type>
class scope
{
    using stored_context_fn_type = std::decay_t<context_fn_type>;

public:
    scope(auditor_ref<auditor_type> auditor, context_fn_type&& context_fn,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_context_fn(std::forward<context_fn_type>(context_fn)),
          m_location(location)
    {
        m_auditor.auditor().record(m_location, docwire::log::scope_enter{}, m_context_fn);
    }

    ~scope()
    {
        m_auditor.auditor().record(m_location, docwire::log::scope_exit{}, m_context_fn);
    }

    scope(const scope&) = delete;
    scope& operator=(const scope&) = delete;

private:
    auditor_ref<auditor_type> m_auditor;
    stored_context_fn_type m_context_fn;
    source_location m_location;
};

template<typename context_fn_type>
class scope<null_auditor, context_fn_type>
{
public:
    constexpr scope(auditor_ref<null_auditor>, context_fn_type&&, source_location = source_location::current()) noexcept {}
    ~scope() noexcept {}
};

template<typename auditor_type, typename context_fn_type>
scope(auditor_ref<auditor_type>, context_fn_type&&) -> scope<auditor_type, context_fn_type>;

template<auditor auditor_type, typename context_fn_type>
scope(auditor_type&, context_fn_type&&) -> scope<auditor_type, context_fn_type>;

} // namespace docwire::audit

#endif
