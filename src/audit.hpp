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

template<typename Auditor>
class holder
{
public:
    constexpr explicit holder(Auditor& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr Auditor& auditor() noexcept { return *m_ptr; }
    constexpr const Auditor& auditor() const noexcept { return *m_ptr; }

private:
    Auditor* m_ptr;
};

template<typename Auditor>
requires std::is_empty_v<Auditor>
class holder<Auditor> : public Auditor
{
public:
    constexpr holder() requires std::default_initializable<Auditor> = default;
    constexpr holder(Auditor&) noexcept {}

    constexpr Auditor& auditor() noexcept { return *this; }
    constexpr const Auditor& auditor() const noexcept { return *this; }
};

template<typename Auditor, typename... Args>
void entry(holder<Auditor> auditor, Args&&... args,
           source_location location = source_location::current())
{
    static_assert(auditor<Auditor>);
    auditor.auditor().record(location, std::forward<Args>(args)...);
}

template<typename Auditor, typename... Args>
class scope
{
public:
    scope(holder<Auditor> auditor, Args&&... args,
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
    holder<Auditor> m_auditor;
    std::tuple<std::decay_t<Args>...> m_args;
    source_location m_location;
};

template<typename... Args>
class scope<null_auditor, Args...>
{
public:
    constexpr scope(holder<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}
};

template<typename Auditor, typename... Args>
scope(holder<Auditor>, Args&&...) -> scope<Auditor, Args...>;

} // namespace docwire::audit

#endif
