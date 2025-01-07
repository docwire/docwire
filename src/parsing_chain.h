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

#ifndef DOCWIRE_PARSING_CHAIN_H
#define DOCWIRE_PARSING_CHAIN_H

#include <memory>
#include "defines.h"
#include "pimpl.h"
#include <vector>

namespace docwire
{

class InputChainElement;

class ChainElement;

template<class T>
concept ChainElementDerived = std::derived_from<T, ChainElement>;

template<typename T>
concept chain_element_derived_ref_qualified = ChainElementDerived<std::remove_reference_t<T>>;

class DllExport ParsingChain : public with_pimpl<ParsingChain>
{
  public:
    ParsingChain(std::shared_ptr<ChainElement> element1, std::shared_ptr<ChainElement> element2);
    ParsingChain(std::shared_ptr<InputChainElement> input, std::shared_ptr<ChainElement> element);
    explicit ParsingChain(std::shared_ptr<ChainElement> element);
    ParsingChain& operator|(std::shared_ptr<ChainElement> element);

    template<chain_element_derived_ref_qualified T>
    ParsingChain& operator|(T&& element)
    {
      return operator|(std::make_shared<T>(std::forward<T>(element)));
    }

    void process(InputChainElement& input);

    const std::vector<std::shared_ptr<ChainElement>>& elements() const;
};

inline std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> chain, std::shared_ptr<ChainElement> element)
{
  chain->operator|(element);
  return chain;
}

template<chain_element_derived_ref_qualified T>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> chain, T&& element)
{
  chain->operator|(std::forward<T>(element));
  return chain;
}

DllExport std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> lhs, ParsingChain&& rhs);

template<chain_element_derived_ref_qualified T>
std::shared_ptr<ParsingChain> operator|(T&& element, ParsingChain&& chain)
{
  return std::make_shared<ParsingChain>(std::make_shared<T>(std::forward<T>(element))) | std::move(chain);
}

inline std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ChainElement> lhs, std::shared_ptr<ChainElement> rhs)
{
  return std::make_shared<ParsingChain>(lhs, rhs);
}

template<chain_element_derived_ref_qualified T>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ChainElement> lhs, T&& rhs)
{
  return std::make_shared<ParsingChain>(lhs, std::make_shared<T>(std::forward<T>(rhs)));
}

template<chain_element_derived_ref_qualified T>
std::shared_ptr<ParsingChain> operator|(T&& lhs, std::shared_ptr<ChainElement> rhs)
{
  return std::make_shared<ParsingChain>(std::make_shared<T>(std::forward<T>(lhs)), rhs);
}

template<chain_element_derived_ref_qualified T, chain_element_derived_ref_qualified U>
ParsingChain operator|(T&& lhs, U&& rhs)
{
  return ParsingChain(std::make_shared<T>(std::forward<T>(lhs)), std::make_shared<U>(std::forward<U>(rhs)));
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
