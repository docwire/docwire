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

  ParsingChain::ParsingChain(std::shared_ptr<ChainElement> element1, std::shared_ptr<ChainElement> element2)
  : first_element(element1), last_element(element2),
    m_input(nullptr)
  {
    first_element->connect(*last_element);
    last_element->set_parent(first_element);
    element_list = {first_element, last_element};
  }

  ParsingChain::ParsingChain(std::shared_ptr<InputChainElement> input, std::shared_ptr<ChainElement> element)
  : m_input(input),
    first_element(element)
  {
    element_list = {first_element};
  }

  ParsingChain&
  ParsingChain::operator|(std::shared_ptr<ChainElement> element_ptr)
  {
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

  void
  ParsingChain::process(InputChainElement& input)
  {
    m_input = std::make_shared<InputChainElement>(input);
    if (last_element && last_element->is_leaf())
    {
      input.process(*first_element);
    }
  }

} // namespace docwire