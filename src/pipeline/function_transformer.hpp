/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_PIPELINE_FUNCTION_TRANSFORMER_HPP
#define DOCWIRE_PIPELINE_FUNCTION_TRANSFORMER_HPP

#include "element_base.hpp"

#include <concepts>
#include <type_traits>
#include <utility>

namespace docwire::pipeline
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

} // namespace docwire::pipeline

#endif // DOCWIRE_PIPELINE_FUNCTION_TRANSFORMER_HPP
