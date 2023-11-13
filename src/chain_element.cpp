/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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

struct ChainElement::Implementation
{
  Implementation()
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(Info &info)>>())
  {}

  Implementation(const Implementation& implementation)
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(Info &info)>>())
  {}

  void connect(const ChainElement &chain_element)
  {
    m_on_new_node_signal->connect([&chain_element](Info &info){chain_element.process(info);});
  }

  void emit(Info &info) const
  {
    (*m_on_new_node_signal)(info);
  }

  std::shared_ptr<boost::signals2::signal<void(Info &info)>> m_on_new_node_signal;
};

ChainElement::ChainElement()
{
  base_impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{}, ImplementationDeleter{}};
}

ChainElement::ChainElement(const ChainElement& element)
: base_impl(new Implementation(*(element.base_impl))),
  m_parent(element.m_parent)
{}

ChainElement&
ChainElement::operator=(const ChainElement &chain_element)
{
  base_impl->m_on_new_node_signal = chain_element.base_impl->m_on_new_node_signal;
  m_parent = chain_element.m_parent;
  return *this;
}

ParsingChain
ChainElement::operator|(ChainElement& chainElement)
{
  return ParsingChain(*this, chainElement);
}

ParsingChain
ChainElement::operator|(ChainElement&& chainElement)
{
  return ParsingChain(*this, chainElement);
}

void
ChainElement::connect(const ChainElement &chain_element)
{
  base_impl->connect(chain_element);
}

void
ChainElement::set_parent(const std::shared_ptr<ChainElement>& chainElement)
{
  m_parent = chainElement;
}

std::shared_ptr<ChainElement>
ChainElement::get_parent() const
{
  return m_parent;
}

void
ChainElement::emit(Info &info) const
{
  base_impl->emit(info);
}

void
ChainElement::ImplementationDeleter::operator()(ChainElement::Implementation *impl)
{
  delete impl;
}

} // namespace docwire
