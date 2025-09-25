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
#include "core_export.h"
#include "pimpl.h"
#include "ref_or_owned.h"

namespace docwire
{

namespace pipeline
{
struct start_processing {};
} // namespace pipeline

class DOCWIRE_CORE_EXPORT ParsingChain : public ChainElement, public with_pimpl<ParsingChain>
{
  public:
    ParsingChain(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs);
    ParsingChain(ParsingChain&& chain);
    ParsingChain& operator=(ParsingChain&& chain);

    void operator()(message_ptr msg);

    bool is_leaf() const override;
    bool is_generator() const override;

    bool is_complete() const;

  protected:
    virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

  private:
    using with_pimpl<ParsingChain>::impl;
};

DOCWIRE_CORE_EXPORT ParsingChain operator|(ref_or_owned<ChainElement> lhs, ref_or_owned<ChainElement> rhs);

inline ParsingChain& operator|=(ParsingChain& lhs, ref_or_owned<ChainElement> rhs)
{
  lhs = std::move(lhs) | rhs;
  return lhs;
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
