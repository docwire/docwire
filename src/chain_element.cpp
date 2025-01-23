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
#include "parsing_chain.h"

namespace docwire
{

template<>
struct pimpl_impl<ChainElement> : pimpl_impl_base
{
  pimpl_impl()
  {}

  void emit(Info &info) const
  {
    ChainElement::continuation continuation = m_callback(info.tag);
    info.skip = continuation == ChainElement::continuation::skip;
    info.cancel = continuation == ChainElement::continuation::stop;
  }

  mutable std::function<ChainElement::continuation(const Tag&)> m_callback;
  std::optional<std::reference_wrapper<ParsingChain>> m_chain;
};

ChainElement::ChainElement()
{
}

ChainElement::continuation ChainElement::operator()(const Tag& tag, std::function<continuation(const Tag&)> callback)
{
  impl().m_callback = callback;
  Info info{tag};
  process(info);
  if (info.cancel)
    return continuation::stop;
  else if (info.skip)
    return continuation::skip;
  else
    return continuation::proceed;
}

void ChainElement::set_chain(ParsingChain& chain)
{
  impl().m_chain = std::ref(chain);
}

std::optional<std::reference_wrapper<ParsingChain>> ChainElement::chain() const
{
  return impl().m_chain;
}

void
ChainElement::emit(Info &info) const
{
  impl().emit(info);
}

} // namespace docwire
