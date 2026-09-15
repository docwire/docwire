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
class auditor_ref
{
public:
    constexpr auditor_ref(Auditor& auditor) noexcept
        : m_ptr(&auditor)
    {}

    constexpr Auditor& auditor() noexcept { return *m_ptr; }
    constexpr const Auditor& auditor() const noexcept { return *m_ptr; }

private:
    Auditor* m_ptr;
};

template<typename Auditor>
requires std::is_empty_v<Auditor>
class auditor_ref<Auditor> : public Auditor
{
public:
    constexpr auditor_ref() requires std::default_initializable<Auditor> = default;
    constexpr auditor_ref(Auditor&) noexcept {}

    constexpr Auditor& auditor() noexcept { return *this; }
    constexpr const Auditor& auditor() const noexcept { return *this; }
};

template<typename Auditor>
class with_auditor : private auditor_ref<Auditor>
{
protected:
    with_auditor() = default;
    explicit with_auditor(auditor_ref<Auditor> auditor)
        : auditor_ref<Auditor>(auditor) {}

    Auditor& auditor() noexcept { return auditor_ref<Auditor>::auditor(); }
    const Auditor& auditor() const noexcept { return auditor_ref<Auditor>::auditor(); }
};

template<typename Auditor, typename... Args>
void entry(auditor_ref<Auditor> auditor_ref_handle, Args&&... args,
           source_location location = source_location::current())
{
    static_assert(auditor<Auditor>);
    auditor_ref_handle.auditor().record(location, std::forward<Args>(args)...);
}

template<auditor Auditor, typename... Args>
void entry(Auditor& auditor, Args&&... args,
           source_location location = source_location::current())
{
    entry(auditor_ref<Auditor>(auditor), std::forward<Args>(args)..., location);
}

template<typename Auditor, typename... Args>
class scope
{
public:
    scope(auditor_ref<Auditor> auditor, Args&&... args,
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
    auditor_ref<Auditor> m_auditor;
    std::tuple<std::decay_t<Args>...> m_args;
    source_location m_location;
};

template<typename... Args>
class scope<null_auditor, Args...>
{
public:
    constexpr scope(auditor_ref<null_auditor>, Args&&..., source_location = source_location::current()) noexcept {}
};

template<typename Auditor, typename... Args>
scope(auditor_ref<Auditor>, Args&&...) -> scope<Auditor, Args...>;

template<auditor Auditor, typename... Args>
scope(Auditor&, Args&&...) -> scope<Auditor, Args...>;

} // namespace docwire::audit

#endif
