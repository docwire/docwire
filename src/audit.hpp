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

template<typename auditor_type, typename... Args>
class entry
{
    static_assert(auditor<auditor_type>);

public:
    entry(auditor_ref<auditor_type> auditor, Args&&... args,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_args(std::forward<Args>(args)...),
          m_location(location)
    {
        std::apply([&](const auto&... args) {
            m_auditor.auditor().record(m_location, args...);
        }, m_args);
    }

    entry(const entry&) = delete;
    entry& operator=(const entry&) = delete;

private:
    auditor_ref<auditor_type> m_auditor;
    std::tuple<std::decay_t<Args>...> m_args;
    source_location m_location;
};

template<typename... Args>
class entry<null_auditor, Args...>
{
public:
    constexpr entry(auditor_ref<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}
};

template<typename auditor_type, typename... Args>
entry(auditor_ref<auditor_type>, Args&&...) -> entry<auditor_type, Args...>;

template<auditor auditor_type, typename... Args>
entry(auditor_type&, Args&&...) -> entry<auditor_type, Args...>;

template<typename auditor_type, typename... Args>
class scope
{
public:
    scope(auditor_ref<auditor_type> auditor, Args&&... args,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_args(std::forward<Args>(args)...),
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
    std::tuple<std::decay_t<Args>...> m_args;
    source_location m_location;
};

template<typename... Args>
class scope<null_auditor, Args...>
{
public:
    constexpr scope(auditor_ref<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}
    ~scope() noexcept {}
};

template<typename auditor_type, typename... Args>
scope(auditor_ref<auditor_type>, Args&&...) -> scope<auditor_type, Args...>;

template<auditor auditor_type, typename... Args>
scope(auditor_type&, Args&&...) -> scope<auditor_type, Args...>;

} // namespace docwire::audit

#endif
