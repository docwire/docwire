#ifndef DOCWIRE_AUDIT_H
#define DOCWIRE_AUDIT_H

#include "log_tags.h"
#include "source_location.h"

#include <concepts>
#include <span>
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

template<typename Auditor>
class handle
{
public:
    constexpr explicit handle(Auditor& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr Auditor& get() const noexcept { return *m_ptr; }

private:
    Auditor* m_ptr;
};

template<>
class handle<null_auditor>
{
public:
    constexpr handle() noexcept = default;
    constexpr handle(null_auditor&) noexcept {}
};

template<typename... Args>
void entry(handle<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}

template<typename Auditor, typename... Args>
void entry(handle<Auditor> auditor, Args&&... args,
           source_location location = source_location::current())
{
    static_assert(auditor<Auditor>);
    auditor.get().record(location, std::forward<Args>(args)...);
}

template<typename Auditor, typename... Args>
class scope
{
public:
    scope(handle<Auditor> auditor, Args&&... args,
          source_location location = source_location::current())
        : m_auditor(auditor),
          m_args(std::forward<Args>(args)...),
          m_location(location)
    {
        std::apply([&](const auto&... args) {
            audit::entry(m_auditor, docwire::log::scope_enter{}, args..., m_location);
        }, m_args);
    }

    ~scope()
    {
        std::apply([&](const auto&... args) {
            audit::entry(m_auditor, docwire::log::scope_exit{}, args..., m_location);
        }, m_args);
    }

    scope(const scope&) = delete;
    scope& operator=(const scope&) = delete;

private:
    handle<Auditor> m_auditor;
    std::tuple<std::decay_t<Args>...> m_args;
    source_location m_location;
};

template<typename... Args>
class scope<null_auditor, Args...>
{
public:
    constexpr scope(handle<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}
};

template<typename Auditor, typename... Args>
scope(handle<Auditor>, Args&&...) -> scope<Auditor, Args...>;

} // namespace docwire::audit

#endif
