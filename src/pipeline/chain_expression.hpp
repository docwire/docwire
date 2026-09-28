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

#ifndef DOCWIRE_PIPELINE_CHAIN_EXPRESSION_H
#define DOCWIRE_PIPELINE_CHAIN_EXPRESSION_H

#include "element_base.hpp"
#include "../log_scope.h"
#include "../ref_or_owned.h"
#include "../serialization_message.h"
#include <memory>
#include <type_traits>
#include <utility>

namespace docwire::pipeline
{

namespace detail
{

/**
 * @brief Selects the public role base of a `chain_expression<L, R>`.
 *
 * | Left        | Right       | Base                    |
 * |-------------|-------------|-------------------------|
 * | source      | transformer | `source_element`        |
 * | transformer | transformer | `transformer_element`   |
 * | transformer | consumer    | `consumer_element`      |
 * | source      | consumer    | `complete_expression`   |
 */
template <typename L, typename R>
struct chain_base_selector
{
    static constexpr bool lhs_source = pipeline::source<L>;
    static constexpr bool lhs_transformer = pipeline::transformer<L>;
    static constexpr bool rhs_transformer = pipeline::transformer<R>;
    static constexpr bool rhs_consumer = pipeline::consumer<R>;

    using type =
        std::conditional_t<lhs_source && rhs_transformer,
            pipeline::source_element<chain_expression<L, R>>,
        std::conditional_t<lhs_transformer && rhs_transformer,
            pipeline::transformer_element<chain_expression<L, R>>,
        std::conditional_t<lhs_transformer && rhs_consumer,
            pipeline::consumer_element<chain_expression<L, R>>,
        std::conditional_t<lhs_source && rhs_consumer,
            pipeline::complete_expression<chain_expression<L, R>>,
        void>>>>;
};

} // namespace detail

/**
 * @brief The role base selected for a `chain_expression<L, R>`.
 *
 * @see chain_expression
 */
template <typename L, typename R>
using chain_base_t =
    typename detail::chain_base_selector<L, R>::type;

/**
 * @brief The internal pipeline node produced by `operator|`.
 *
 * This expression-tree node encodes, in its type, the static composition of two
 * pipeline elements. It is an implementation detail and is almost never written
 * explicitly by users; its presence is detected by the `pipeline::chain`
 * concept.
 *
 * @see pipeline::chain
 * @see operator|
 */
template <typename L, typename R>
class chain_expression : public chain_base_t<L, R>
{
  public:
    chain_expression(ref_or_owned<L> lhs_element, ref_or_owned<R> rhs_element)
      : m_lhs_element{std::move(lhs_element)}, m_rhs_element{std::move(rhs_element)}
    {}

    chain_expression(chain_expression&& chain) = default;
    chain_expression& operator=(chain_expression&& chain) = default;

    void operator()(message_ptr msg)
    {
      DOCWIRE_LOG_SCOPE(msg);
      operator()(std::move(msg),
        {
          [](message_ptr) { return continuation::proceed; },
          [](message_ptr) { return continuation::proceed; }
        });
    }

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
      DOCWIRE_LOG_SCOPE(msg);

      auto downstream = emit_message;

      auto lhs_callback = [this, downstream](message_ptr msg)
      {
        DOCWIRE_LOG_SCOPE(msg);

        auto back_to_left = [this, lhs_callback, back_upstream = downstream.m_back](message_ptr msg)
        {
          DOCWIRE_LOG_SCOPE(msg);
          return m_lhs_element.get()(std::move(msg),
              message_callbacks{lhs_callback, back_upstream});
        };

        return m_rhs_element.get()(std::move(msg),
            message_callbacks{downstream.m_further, back_to_left});
      };

      return m_lhs_element.get()(std::move(msg),
          message_callbacks{lhs_callback, downstream.m_back});
    }

  private:
    ref_or_owned<L> m_lhs_element;
    ref_or_owned<R> m_rhs_element;
};

/**
 * @brief Composes two pipeline elements into a lazily evaluated chain.
 *
 * The operator only participates for grammatically valid connections: a source
 * or transformer on the left, and a transformer or consumer on the right. A
 * `source | consumer` combination produces a complete, immediately executed
 * pipeline.
 *
 * @see chain_expression
 * @see complete_expression
 */
template <typename L, typename R>
    requires source_or_transformer<L>
          && transformer_or_consumer<R>
auto operator|(L&& lhs, R&& rhs)
{
    using lhs_t = std::remove_cvref_t<L>;
    using rhs_t = std::remove_cvref_t<R>;

    chain_expression<lhs_t, rhs_t> chain{
        ref_or_owned<lhs_t>{std::forward<L>(lhs)},
        ref_or_owned<rhs_t>{std::forward<R>(rhs)}
    };

    if constexpr (pipeline::source<L> && pipeline::consumer<R>)
    {
        chain(std::make_shared<message<pipeline::start_processing>>(
            pipeline::start_processing{}));
    }

    return chain;
}

} // namespace docwire::pipeline

#endif //DOCWIRE_PIPELINE_CHAIN_EXPRESSION_H
