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
#include <functional>
#include "parsing_chain.h"

namespace docwire
{

template<>
struct pimpl_impl<ParsingChain> : with_pimpl_owner<ParsingChain>
{
  pimpl_impl(ParsingChain& owner, ref_or_owned<ChainElement> lhs_element, ref_or_owned<ChainElement> rhs_element)
    : with_pimpl_owner{owner}, m_lhs_element{lhs_element}, m_rhs_element{rhs_element}
  {
    m_lhs_element.get().set_chain(owner);
    m_rhs_element.get().set_chain(owner);
    m_lhs_element.get().connect(m_rhs_element.get());
  }

  ParsingChain& top_chain()
  {
    std::optional<std::reference_wrapper<ParsingChain>> chain = owner().chain();
    if (chain)
      return chain->get().top_chain();
    else
      return owner();
  }

  ref_or_owned<ChainElement> m_lhs_element;
  ref_or_owned<ChainElement> m_rhs_element;
};

ParsingChain::ParsingChain(ref_or_owned<ChainElement> lhs_element, ref_or_owned<ChainElement> rhs_element)
: with_pimpl<ParsingChain>(lhs_element, rhs_element)
{}

ParsingChain::ParsingChain(ParsingChain&& other)
  : with_pimpl<ParsingChain>(std::move(static_cast<with_pimpl<ParsingChain>&>(other)))
{
  impl().m_lhs_element.get().set_chain(*this);
  impl().m_rhs_element.get().set_chain(*this);
}

ParsingChain& ParsingChain::operator=(ParsingChain&& other)
{
  with_pimpl<ParsingChain>::operator=(std::move(static_cast<with_pimpl<ParsingChain>&>(other)));
  impl().m_lhs_element.get().set_chain(*this);
  impl().m_rhs_element.get().set_chain(*this);
  return *this;
}

void ParsingChain::process(Info &info)
{
  impl().m_lhs_element.get().process(info);
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

void ParsingChain::connect(ChainElement& chain_element)
{
  impl().m_rhs_element.get().connect(chain_element);
}

ParsingChain& ParsingChain::top_chain()
{
  return impl().top_chain();
}

ParsingChain operator|(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs)
{
  ParsingChain chain{lhs, rhs};
  if (chain.is_complete())
  {
    Info info{tag::start_processing{}};
    chain.process(info);
  }
  return chain;
}

} // namespace docwire