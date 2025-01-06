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
#include "input.h"
#include "pimpl.h"
#include <memory>
#include "parsing_chain.h"

namespace docwire
{

template<>
struct pimpl_impl<ParsingChain> : pimpl_impl_base
{
  pimpl_impl(std::shared_ptr<ChainElement> first_element, std::shared_ptr<ChainElement> last_element, std::shared_ptr<InputChainElement> input)
    : m_first_element{first_element}, m_last_element{last_element}, m_input{input}
  {
    if (last_element)
    {
      m_element_list = {m_first_element, m_last_element};
      first_element->connect(*last_element);
      last_element->set_parent(first_element);
    }
    else
      m_element_list = {m_first_element};
  }

  void append_element(std::shared_ptr<ChainElement> element_ptr)
  {
    m_element_list.push_back(element_ptr);
    if (m_last_element)
    {
      m_last_element->connect(*element_ptr);
      element_ptr->set_parent(m_last_element);
    }
    else
    {
      m_first_element->connect(*element_ptr);
      element_ptr->set_parent(m_first_element);
    }
    m_last_element = element_ptr;

    if (m_last_element->is_leaf())
    {
      if (m_input)
      {
        m_input->process(*m_first_element);
      }
    }
  }

  void process(InputChainElement& input)
  {
    m_input = std::make_shared<InputChainElement>(input);
    if (m_last_element && m_last_element->is_leaf())
    {
      input.process(*m_first_element);
    }
  }

  std::shared_ptr<InputChainElement> m_input;
  std::shared_ptr<ChainElement> m_first_element;
  std::shared_ptr<ChainElement> m_last_element;
  std::vector<std::shared_ptr<ChainElement>> m_element_list;
};

  ParsingChain::ParsingChain(std::shared_ptr<ChainElement> element1, std::shared_ptr<ChainElement> element2)
  : with_pimpl(element1, element2, nullptr)
  {
  }

  ParsingChain::ParsingChain(std::shared_ptr<InputChainElement> input, std::shared_ptr<ChainElement> element)
  : with_pimpl(element, nullptr, input)
  {
  }

  ParsingChain::ParsingChain(std::shared_ptr<ChainElement> element)
  : with_pimpl(element, nullptr, nullptr)
  {
  }

  ParsingChain&
  ParsingChain::operator|(std::shared_ptr<ChainElement> element_ptr)
  {
    impl().append_element(element_ptr);
    return *this;
  }

  void
  ParsingChain::process(InputChainElement& input)
  {
    impl().process(input);
  }

const std::vector<std::shared_ptr<ChainElement>>& ParsingChain::elements() const
{
  return impl().m_element_list;
}

std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> lhs, ParsingChain&& rhs)
{
  for (auto element : rhs.elements())
  {
    element->disconnect_all();
    lhs->operator|(element);
  }
  return lhs;
}

} // namespace docwire