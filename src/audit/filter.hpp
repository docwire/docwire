#ifndef DOCWIRE_AUDIT_FILTER_HPP
#define DOCWIRE_AUDIT_FILTER_HPP

#include "detail/chaining.hpp"
#include "detail/filter_rules.hpp"
#include "detail/value_conversion.hpp"
#include "../source_location.h"

#include <cstdio>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace docwire::audit::filter
{

template<typename Next = void>
class by_spec
{
    template<typename> friend class by_spec;

public:
    explicit by_spec(std::string filter_spec)
        : m_filter_spec(detail::parse_filter(std::move(filter_spec)))
    {
    }

    template<typename R>
    auto operator|(R&& next) &&
    {
        return std::move(*this).with_next(std::forward<R>(next));
    }

    template<typename... Args>
        requires (!std::is_void_v<Next>)
    void record(source_location location, Args&&... args)
    {
        const auto tags = detail::collect_tags<Args...>();
        std::span<const std::string_view> tags_span{tags};

        std::fprintf(stderr, "[filter::record] this=%p\n",
                     reinterpret_cast<void*>(this));

        if (detail::matches(m_filter_spec, location, tags_span))
        {
            m_next.record(location, std::forward<Args>(args)...);
        }
    }

    template<typename R>
    auto with_next(R&& next) &&
    {
        if constexpr (std::is_void_v<Next>)
        {
            return by_spec<std::decay_t<R>>(
                std::move(m_filter_spec),
                std::forward<R>(next));
        }
        else
        {
            auto new_next = std::move(m_next).with_next(std::forward<R>(next));
            return by_spec<decltype(new_next)>(
                std::move(m_filter_spec),
                std::move(new_next));
        }
    }

private:
    template<typename NextParam>
    explicit by_spec(detail::filter_spec spec, NextParam&& next)
        requires (!std::is_void_v<Next> &&
                  std::constructible_from<detail::next_storage_t<Next>, NextParam&&>)
        : m_filter_spec(std::move(spec)),
          m_next(std::forward<NextParam>(next))
    {
    }

    detail::filter_spec m_filter_spec;
    detail::next_storage_t<Next> m_next;
};

by_spec(std::string) -> by_spec<void>;

} // namespace docwire::audit::filter

#endif
