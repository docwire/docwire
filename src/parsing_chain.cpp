/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "chain_element.h"
#include "pimpl.h"
#include "message.h"
#include "parsing_chain.h"

namespace docwire
{

template<>
struct pimpl_impl<ParsingChain> : with_pimpl_owner<ParsingChain>
{
  pimpl_impl(ParsingChain& owner, ref_or_owned<ChainElement> lhs_element, ref_or_owned<ChainElement> rhs_element)
    : with_pimpl_owner{owner}, m_lhs_element{lhs_element}, m_rhs_element{rhs_element}
  {}

  ref_or_owned<ChainElement> m_lhs_element;
  ref_or_owned<ChainElement> m_rhs_element;
};

ParsingChain::ParsingChain(ref_or_owned<ChainElement> lhs_element, ref_or_owned<ChainElement> rhs_element)
: with_pimpl<ParsingChain>(lhs_element, rhs_element)
{}

ParsingChain::ParsingChain(ParsingChain&& other)
  : with_pimpl<ParsingChain>(std::move(static_cast<with_pimpl<ParsingChain>&>(other)))
{}

ParsingChain& ParsingChain::operator=(ParsingChain&& other)
{
  with_pimpl<ParsingChain>::operator=(std::move(static_cast<with_pimpl<ParsingChain>&>(other)));
  return *this;
}

void ParsingChain::operator()(message_ptr msg)
{
  operator()(std::move(msg),
  {
    [](message_ptr) { return continuation::proceed; },
    [this](message_ptr msg)
    {
      operator()(std::move(msg));
      return continuation::proceed;
    }
  });
}

continuation ParsingChain::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  auto lhs_callback = [this, &rhs_callbacks = emit_message](message_ptr msg)
  {
    return impl().m_rhs_element.get()(std::move(msg), rhs_callbacks);
  };
  return impl().m_lhs_element.get()( std::move(msg),
    {
      lhs_callback,
      [emit_message](message_ptr msg) { return emit_message.back(std::move(msg)); }
    });
}

bool ParsingChain::is_leaf() const
{
  return impl().m_rhs_element.get().is_leaf();
}

bool ParsingChain::is_generator() const
{
  return impl().m_lhs_element.get().is_generator();
}

bool ParsingChain::is_complete() const
{
  return is_generator() && is_leaf();
}

ParsingChain operator|(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs)
{
  ParsingChain chain{lhs, rhs};
  if (chain.is_complete())
  {
    chain(std::make_shared<message<pipeline::start_processing>>(pipeline::start_processing{}));
  }
  return chain;
}

} // namespace docwire