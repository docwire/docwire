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

namespace pipeline
{
struct start_processing {};
} // namespace pipeline

class parsing_chain : public chain_element
{
  public:
    parsing_chain(ref_or_owned<chain_element> lhs_element, ref_or_owned<chain_element> rhs_element);
    parsing_chain(parsing_chain&& chain) = default;
    parsing_chain& operator=(parsing_chain&& chain) = default;

    void operator()(message_ptr msg);

    bool is_leaf() const override;
    bool is_generator() const override;

    bool is_complete() const;

  protected:
    virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

  private:
    ref_or_owned<chain_element> m_lhs_element;
    ref_or_owned<chain_element> m_rhs_element;
};

inline parsing_chain::parsing_chain(ref_or_owned<chain_element> lhs_element, ref_or_owned<chain_element> rhs_element)
  : m_lhs_element{std::move(lhs_element)}, m_rhs_element{std::move(rhs_element)}
{}

inline void parsing_chain::operator()(message_ptr msg)
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

inline continuation parsing_chain::operator()(message_ptr msg, const message_callbacks& emit_message)
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

inline bool parsing_chain::is_leaf() const
{
  return m_rhs_element.get().is_leaf();
}

inline bool parsing_chain::is_generator() const
{
  return m_lhs_element.get().is_generator();
}

inline bool parsing_chain::is_complete() const
{
  return is_generator() && is_leaf();
}

inline parsing_chain operator|(ref_or_owned<chain_element> lhs, ref_or_owned<chain_element> rhs)
{
  parsing_chain chain{std::move(lhs), std::move(rhs)};
  if (chain.is_complete())
  {
    chain(std::make_shared<message<pipeline::start_processing>>(pipeline::start_processing{}));
  }
  return chain;
}

inline parsing_chain& operator|=(parsing_chain& lhs, ref_or_owned<chain_element> rhs)
{
  lhs = std::move(lhs) | rhs;
  return lhs;
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
