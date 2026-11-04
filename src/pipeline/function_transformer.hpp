/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_PIPELINE_FUNCTION_TRANSFORMER_HPP
#define DF0_PIPELINE_FUNCTION_TRANSFORMER_HPP

#include "element_base.hpp"

#include <concepts>
#include <type_traits>
#include <utility>

namespace df0::pipeline
{

/**
 * @brief Wraps a callable into a statically typed transformer element.
 *
 * The callable type is preserved by value. No `std::function` is used.
 *
 * @tparam Func The callable type to wrap.
 */
template <typename Func>
class function_transformer
    : public pipeline::transformer_element<function_transformer<Func>>
{
public:
    function_transformer(Func func)
        : m_func{std::move(func)}
    {
    }

    continuation operator()(message_ptr msg,
                            const message_callbacks& emit_message)
    {
        return m_func(std::move(msg), emit_message);
    }

private:
    Func m_func;
};

template <typename Func>
function_transformer(Func) -> function_transformer<Func>;

/**
 * @brief Pipes a callable transformer function into a static chain.
 *
 * The callable is wrapped into a `function_transformer`, preserving its type
 * without converting it to `std::function`.
 */
template <typename Chain, typename Func>
    requires source_or_transformer<Chain>
          && (!element<Func>)
          && std::invocable<std::remove_cvref_t<Func>&,
                            message_ptr,
                            const message_callbacks&>
auto operator|(Chain&& chain, Func&& func)
{
    using callable_type = std::remove_cvref_t<Func>;

    return std::forward<Chain>(chain)
         | function_transformer<callable_type>{std::forward<Func>(func)};
}

} // namespace df0::pipeline

#endif // DF0_PIPELINE_FUNCTION_TRANSFORMER_HPP
