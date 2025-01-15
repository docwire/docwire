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

#include <boost/signals2.hpp>

#include "chain_element.h"
#include "parsing_chain.h"

namespace docwire
{

template<>
struct pimpl_impl<ChainElement> : pimpl_impl_base
{
  pimpl_impl()
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(Info &info)>>())
  {}

  void connect(ChainElement& chain_element)
  {
    m_on_new_node_signal->connect([&chain_element](Info &info){chain_element.process(info);});
  }

  void emit(Info &info) const
  {
    (*m_on_new_node_signal)(info);
  }

  std::shared_ptr<boost::signals2::signal<void(Info &info)>> m_on_new_node_signal;
  std::optional<std::reference_wrapper<ParsingChain>> m_chain;
};

ChainElement::ChainElement()
{
}

void
ChainElement::connect(ChainElement& chain_element)
{
  impl().connect(chain_element);
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
