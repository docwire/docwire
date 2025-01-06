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

class DllExport ParsingChain : public with_pimpl<ParsingChain>
{
  public:
    ParsingChain(std::shared_ptr<ChainElement> element1, std::shared_ptr<ChainElement> element2);
    ParsingChain(std::shared_ptr<InputChainElement> input, std::shared_ptr<ChainElement> element);
    explicit ParsingChain(std::shared_ptr<ChainElement> element);
    ParsingChain& operator|(std::shared_ptr<ChainElement> element);

    template<ChainElementDerived T>
    ParsingChain& operator|(T&& element)
    {
      return operator|(std::make_shared<T>(std::move(element)));
    }

    void process(InputChainElement& input);

    const std::vector<std::shared_ptr<ChainElement>>& elements() const;
};

inline std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> chain, std::shared_ptr<ChainElement> element)
{
  chain->operator|(element);
  return chain;
}

template<ChainElementDerived T>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> chain, T&& element)
{
  chain->operator|(std::move(element));
  return chain;
}

DllExport std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ParsingChain> lhs, ParsingChain&& rhs);

template<ChainElementDerived T>
std::shared_ptr<ParsingChain> operator|(T&& element, ParsingChain&& chain)
{
  return std::make_shared<ParsingChain>(std::make_shared<T>(std::move(element))) | std::forward<ParsingChain>(chain);
}

inline std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ChainElement> lhs, std::shared_ptr<ChainElement> rhs)
{
  return std::make_shared<ParsingChain>(lhs, rhs);
}

template<ChainElementDerived T>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<ChainElement> lhs, T&& rhs)
{
  return std::make_shared<ParsingChain>(lhs, std::make_shared<T>(std::move(rhs)));
}

template<ChainElementDerived T>
std::shared_ptr<ParsingChain> operator|(T&& lhs, std::shared_ptr<ChainElement> rhs)
{
  return std::make_shared<ParsingChain>(std::make_shared<T>(std::move(lhs)), rhs);
}

template<ChainElementDerived T, ChainElementDerived U>
ParsingChain operator|(T&& lhs, U&& rhs)
{
  return ParsingChain(std::make_shared<T>(std::move(lhs)), std::make_shared<U>(std::move(rhs)));
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
