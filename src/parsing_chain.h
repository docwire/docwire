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
#include <utility>

namespace docwire
{

template <typename L, typename R>
class parsing_chain : public chain_element<parsing_chain<L, R>>
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

namespace pipeline
{

template <typename L, typename R>
struct is_generator<parsing_chain<L, R>>
    : is_generator<L> {};

template <typename L, typename R>
struct is_leaf<parsing_chain<L, R>>
    : is_leaf<R> {};

} // namespace pipeline

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
