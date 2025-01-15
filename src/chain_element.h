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

#ifndef DOCWIRE_CHAIN_ELEMENT_H
#define DOCWIRE_CHAIN_ELEMENT_H

#include "parser.h"
#include "pimpl.h"

namespace docwire
{

class ParsingChain;

class DllExport ChainElement : public with_pimpl<ChainElement>
{
public:
  ChainElement();
  ChainElement(ChainElement&&) = default;
  virtual ~ChainElement() = default;
  ChainElement& operator=(ChainElement&&) = default;

  /**
   * @brief Connects next object to the end of chain
   * @param chain_element
   */
  virtual void connect(ChainElement& chain_element);

  /**
   * @brief Emits signal with Info object to the next element
   * @param info
   */
  void emit(Info &info) const;

  /**
   * @brief Check if ChainElement is a leaf(last element which doesn't emit any signals). At this moment only Exporters are leafs.
   * @return true if leaf
   */
  virtual bool is_leaf() const = 0;

  virtual bool is_generator() const { return false; }

  /**
   * @brief Start processing
   * @param info
   */
  virtual void process(Info &info) = 0;

  /**
   * @brief Set parsing chain that this element belongs to
   * @param chain
   */
  void set_chain(ParsingChain& chainElement);

  std::optional<std::reference_wrapper<ParsingChain>> chain() const;
};

template<typename T>
concept ParsingChainOrChainElement = std::derived_from<T, ParsingChain> || std::derived_from<T, ChainElement>;

template<typename T>
concept parsing_chain_or_chain_element_ref_qualified = ParsingChainOrChainElement<std::remove_reference_t<T>>;

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
