#ifndef DOCWIRE_AUDIT_SERIALIZE_JSON_HPP
#define DOCWIRE_AUDIT_SERIALIZE_JSON_HPP

#include "audit/detail/chaining.hpp"
#include "audit/detail/value_conversion.hpp"
#include "serialization_base.h"
#include "source_location.h"

#include <type_traits>
#include <utility>

namespace docwire::audit::serialize
{

template<typename Next = void>
class json
{
    template<typename> friend class json;

    template<typename L, typename R>
        requires requires(L l, R r) { std::move(l).with_next(std::move(r)); }
    friend auto operator|(L&& left, R&& right)
    {
        return std::forward<L>(left).with_next(std::forward<R>(right));
    }

public:
    json() requires std::is_void_v<Next> = default;

    template<typename R>
    auto with_next(R&& next) &&
    {
        return json<std::decay_t<R>>(std::forward<R>(next));
    }

    template<typename... Args>
        requires (!std::is_void_v<Next>)
    void record(source_location location, Args&&... args)
    {
        auto context = detail::build_context(std::forward<Args>(args)...);
        auto record_object = detail::create_record_object(location, std::move(context));
        m_next.record(location, std::move(record_object));
    }

private:
    template<typename NextParam>
    explicit json(NextParam&& next)
        requires (!std::is_void_v<Next> &&
                  std::constructible_from<detail::next_storage_t<Next>, NextParam&&>)
        : m_next(std::forward<NextParam>(next))
    {
    }

    detail::next_storage_t<Next> m_next;
};

} // namespace docwire::audit::serialize

#endif
