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

#include "chain_element.h"
#include "defines.h"
#include "pimpl.h"
#include "ref_or_owned.h"

namespace docwire
{

class DllExport ParsingChain : public ChainElement, public with_pimpl<ParsingChain>
{
  public:
    ParsingChain(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs);
    ParsingChain(ParsingChain&& chain);
    ParsingChain& operator=(ParsingChain&& chain);

    void operator()(const Tag& tag);

    bool is_leaf() const override;
    bool is_generator() const override;

    ParsingChain& top_chain();
    bool is_complete() const;

  protected:
    void process(Info &info) override;

  private:
    using with_pimpl<ParsingChain>::impl;
};

DllExport ParsingChain operator|(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs);

inline ParsingChain& operator|=(ParsingChain& lhs, ref_or_owned<ChainElement> rhs)
{
  lhs = std::move(lhs) | rhs;
  return lhs;
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
