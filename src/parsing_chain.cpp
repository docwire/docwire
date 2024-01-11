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

#include "input.h"
#include "parsing_chain.h"

namespace docwire
{
  ParsingChain::ParsingChain(ChainElement& element1, ChainElement& element2)
  : first_element(std::shared_ptr<ChainElement>(element1.clone())),
    last_element(std::shared_ptr<ChainElement>(element2.clone())),
    m_input(nullptr)
  {
    first_element->connect(*last_element);
    last_element->set_parent(first_element);
    element_list = {first_element, last_element};
  }

  ParsingChain::ParsingChain(const ChainElement& element)
  : first_element(std::shared_ptr<ChainElement>(element.clone())),
    m_input(nullptr)
  {
    element_list = {first_element};
  }

  ParsingChain::ParsingChain(const InputBase &input, ChainElement& element)
  : m_input(&input),
    first_element(std::shared_ptr<ChainElement>(element.clone()))
  {
    element_list = {first_element};
  }

  ParsingChain&
  ParsingChain::operator|(const ChainElement& element)
  {
    auto element_ptr = std::shared_ptr<ChainElement>(element.clone());
    element_list.push_back(element_ptr);
    if (last_element)
    {
      last_element->connect(*element_ptr);
      element_ptr->set_parent(last_element);
    }
    else
    {
      first_element->connect(*element_ptr);
      element_ptr->set_parent(first_element);
    }
    last_element = element_ptr;

    if (last_element->is_leaf())
    {
      if (m_input)
      {
        m_input->process(*first_element);
      }
    }
    return *this;
  }

  ParsingChain&
  ParsingChain::operator|(ChainElement&& element)
  {
    return operator|(element);
  }

  void
  ParsingChain::process(InputBase& input)
  {
    m_input = &input;
    if (last_element && last_element->is_leaf())
    {
      input.process(*first_element);
    }
  }

} // namespace docwire