#ifndef DOCWIRE_AUDIT_CHAIN_HPP
#define DOCWIRE_AUDIT_CHAIN_HPP

#include <utility>

namespace docwire::audit
{

template<typename Left, typename Right>
    requires requires(Left left, Right right)
    {
        std::move(left).with_next(std::move(right));
    }
auto operator|(Left left, Right right)
{
    return std::move(left).with_next(std::move(right));
}

} // namespace docwire::audit

#endif
