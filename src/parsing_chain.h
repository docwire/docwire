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

#ifndef DOCWIRE_PARSING_CHAIN_H
#define DOCWIRE_PARSING_CHAIN_H

#include "chain_element.h"
#include "core_export.h"
#include "log_scope.h"
#include "ref_or_owned.h"
#include "serialization_message.h"
#include <memory>
#include <type_traits>
#include <utility>

namespace docwire
{

namespace pipeline::detail
{

/**
 * @brief Selects the public role base of a `parsing_chain<L, R>`.
 *
 * | Left        | Right       | Base                  |
 * |-------------|-------------|-----------------------|
 * | source      | transformer | `source_element`      |
 * | transformer | transformer | `transformer_element` |
 * | transformer | consumer    | `consumer_element`    |
 * | source      | consumer    | `complete_pipeline`   |
 */
template <typename L, typename R>
struct parsing_chain_base_selector
{
    static constexpr bool lhs_source = pipeline::source<L>;
    static constexpr bool lhs_transformer = pipeline::transformer<L>;
    static constexpr bool rhs_transformer = pipeline::transformer<R>;
    static constexpr bool rhs_consumer = pipeline::consumer<R>;

    using type =
        std::conditional_t<lhs_source && rhs_transformer,
            pipeline::source_element<parsing_chain<L, R>>,
        std::conditional_t<lhs_transformer && rhs_transformer,
            pipeline::transformer_element<parsing_chain<L, R>>,
        std::conditional_t<lhs_transformer && rhs_consumer,
            pipeline::consumer_element<parsing_chain<L, R>>,
        std::conditional_t<lhs_source && rhs_consumer,
            pipeline::complete_pipeline<parsing_chain<L, R>>,
        void>>>>;
};

} // namespace pipeline::detail

/**
 * @brief The role base selected for a `parsing_chain<L, R>`.
 *
 * @see parsing_chain
 */
template <typename L, typename R>
using parsing_chain_base_t =
    typename pipeline::detail::parsing_chain_base_selector<L, R>::type;

template <typename L, typename R>
class parsing_chain : public parsing_chain_base_t<L, R>
{
  public:
    parsing_chain(ref_or_owned<L> lhs_element, ref_or_owned<R> rhs_element)
      : m_lhs_element{std::move(lhs_element)}, m_rhs_element{std::move(rhs_element)}
    {}

    parsing_chain(parsing_chain&& chain) = default;
    parsing_chain& operator=(parsing_chain&& chain) = default;

    void operator()(message_ptr msg)
    {
      DOCWIRE_LOG_SCOPE(msg);
      operator()(std::move(msg),
      {
        [](message_ptr msg)
        {
          DOCWIRE_LOG_SCOPE(msg);
          return continuation::proceed;
        },
        [this](message_ptr msg)
        {
          DOCWIRE_LOG_SCOPE(msg);
          operator()(std::move(msg));
          return continuation::proceed;
        }
      });
    }

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
      DOCWIRE_LOG_SCOPE(msg);
      auto lhs_callback = [this, &rhs_callbacks = emit_message](message_ptr msg)
      {
        DOCWIRE_LOG_SCOPE(msg);
        return m_rhs_element.get()(std::move(msg), rhs_callbacks);
      };
      return m_lhs_element.get()(std::move(msg),
        {
          lhs_callback,
          [emit_message](message_ptr msg)
          {
            DOCWIRE_LOG_SCOPE(msg);
            return emit_message.back(std::move(msg));
          }
        });
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
 * @see parsing_chain
 * @see complete_pipeline
 */
template <typename L, typename R>
    requires pipeline::source_or_transformer<L>
          && pipeline::transformer_or_consumer<R>
auto operator|(L&& lhs, R&& rhs)
{
    using lhs_t = std::remove_cvref_t<L>;
    using rhs_t = std::remove_cvref_t<R>;

    parsing_chain<lhs_t, rhs_t> chain{
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

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
