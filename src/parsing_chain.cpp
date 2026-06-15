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

#include "chain_element.h"
#include "pimpl.h"
#include "log_scope.h"
#include "message.h"
#include "parsing_chain.h"
#include "serialization_message.h"

namespace docwire
{

template<>
struct pimpl_impl<parsing_chain> : with_pimpl_owner<parsing_chain>
{
  pimpl_impl(parsing_chain& owner, ref_or_owned<chain_element> lhs_element, ref_or_owned<chain_element> rhs_element)
    : with_pimpl_owner{owner}, m_lhs_element{lhs_element}, m_rhs_element{rhs_element}
  {}

  ref_or_owned<chain_element> m_lhs_element;
  ref_or_owned<chain_element> m_rhs_element;
};

parsing_chain::parsing_chain(ref_or_owned<chain_element> lhs_element, ref_or_owned<chain_element> rhs_element)
: with_pimpl<parsing_chain>(lhs_element, rhs_element)
{}

parsing_chain::parsing_chain(parsing_chain&& other)
  : with_pimpl<parsing_chain>(std::move(static_cast<with_pimpl<parsing_chain>&>(other)))
{}

parsing_chain& parsing_chain::operator=(parsing_chain&& other)
{
  with_pimpl<parsing_chain>::operator=(std::move(static_cast<with_pimpl<parsing_chain>&>(other)));
  return *this;
}

void parsing_chain::operator()(message_ptr msg)
{
  log_scope(msg);
  operator()(std::move(msg),
  {
    [](message_ptr msg)
    {
      log_scope(msg);
      return continuation::proceed;
    },
    [this](message_ptr msg)
    {
      log_scope(msg);
      operator()(std::move(msg));
      return continuation::proceed;
    }
  });
}

continuation parsing_chain::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  log_scope(msg);
  auto lhs_callback = [this, &rhs_callbacks = emit_message](message_ptr msg)
  {
    log_scope(msg);
    return impl().m_rhs_element.get()(std::move(msg), rhs_callbacks);
  };
  return impl().m_lhs_element.get()( std::move(msg),
    {
      lhs_callback,
      [emit_message](message_ptr msg)
      {
        log_scope(msg);
        return emit_message.back(std::move(msg));
      }
    });
}

bool parsing_chain::is_leaf() const
{
  return impl().m_rhs_element.get().is_leaf();
}

bool parsing_chain::is_generator() const
{
  return impl().m_lhs_element.get().is_generator();
}

bool parsing_chain::is_complete() const
{
  return is_generator() && is_leaf();
}

parsing_chain operator|(ref_or_owned<chain_element> lhs, ref_or_owned<chain_element> rhs)
{
  parsing_chain chain{lhs, rhs};
  if (chain.is_complete())
  {
    chain(std::make_shared<message<pipeline::start_processing>>(pipeline::start_processing{}));
  }
  return chain;
}

} // namespace docwire