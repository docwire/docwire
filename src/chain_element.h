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

#include "defines.h"
#include "pimpl.h"
#include "tags.h"

namespace docwire
{

class ParsingChain;

struct DllExport Info
{
  Tag tag;
  bool cancel = false; //!< cancel flag. If set true then parsing process will be stopped.
  bool skip = false; //!< skip flag. If set true then tag will be skipped.

  explicit Info(const Tag& tag)
    : tag(tag)
  {}
};

class DllExport ChainElement : public with_pimpl<ChainElement>
{
public:
  ChainElement();
  ChainElement(ChainElement&&) = default;
  virtual ~ChainElement() = default;
  ChainElement& operator=(ChainElement&&) = default;

  enum class continuation { proceed, skip, stop };
  continuation operator()(const Tag& tag, std::function<continuation(const Tag&)> callback);

  /**
   * @brief Check if ChainElement is a leaf (last element which doesn't produce any tags). At this moment only Exporters are leafs.
   * @return true if leaf
   */
  virtual bool is_leaf() const = 0;

  virtual bool is_generator() const { return false; }

  /**
   * @brief Set parsing chain that this element belongs to
   * @param chain
   */
  void set_chain(ParsingChain& chainElement);

  std::optional<std::reference_wrapper<ParsingChain>> chain() const;

protected:
  void emit(Info &info) const;
  virtual void process(Info &info) = 0;
};

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
