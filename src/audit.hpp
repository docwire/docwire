#ifndef DOCWIRE_AUDIT_H
#define DOCWIRE_AUDIT_H

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

template<typename auditor_type, typename context_fn_type>
class entry
{
    static_assert(auditor<auditor_type>);

public:
    entry(auditor_ref<auditor_type> auditor, context_fn_type&& context_fn,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_location(location)
    {
        std::apply([&](const auto&... args) {
            m_auditor.auditor().record(m_location, args...);
        }, std::forward<context_fn_type>(context_fn)());
    }

    entry(const entry&) = delete;
    entry& operator=(const entry&) = delete;

private:
    auditor_ref<auditor_type> m_auditor;
    source_location m_location;
};

template<typename context_fn_type>
class entry<null_auditor, context_fn_type>
{
public:
    constexpr entry(auditor_ref<null_auditor>, context_fn_type&&, source_location = source_location::current()) noexcept {}
};

template<typename auditor_type, typename context_fn_type>
entry(auditor_ref<auditor_type>, context_fn_type&&) -> entry<auditor_type, context_fn_type>;

template<auditor auditor_type, typename context_fn_type>
entry(auditor_type&, context_fn_type&&) -> entry<auditor_type, context_fn_type>;

template<typename auditor_type, typename context_fn_type>
class scope
{
    using context_tuple_type = std::decay_t<std::invoke_result_t<context_fn_type&>>;

public:
    scope(auditor_ref<auditor_type> auditor, context_fn_type&& context_fn,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_args(std::forward<context_fn_type>(context_fn)()),
          m_location(location)
    {
        std::apply([&](const auto&... args) {
            m_auditor.auditor().record(m_location, docwire::log::scope_enter{}, args...);
        }, m_args);
    }

    ~scope()
    {
        std::apply([&](const auto&... args) {
            m_auditor.auditor().record(m_location, docwire::log::scope_exit{}, args...);
        }, m_args);
    }

    scope(const scope&) = delete;
    scope& operator=(const scope&) = delete;

private:
    auditor_ref<auditor_type> m_auditor;
    context_tuple_type m_args;
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
