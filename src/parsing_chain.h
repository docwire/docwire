/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
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

class DOCWIRE_CORE_EXPORT parsing_chain : public chain_element, public with_pimpl<parsing_chain>
{
  public:
    parsing_chain(ref_or_owned<chain_element> lhs, ref_or_owned<chain_element> rhs);
    parsing_chain(parsing_chain&& chain);
    parsing_chain& operator=(parsing_chain&& chain);

    void operator()(message_ptr msg);

    bool is_leaf() const override;
    bool is_generator() const override;

    bool is_complete() const;

  protected:
    virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

  private:
    using with_pimpl<parsing_chain>::impl;
};

DOCWIRE_CORE_EXPORT parsing_chain operator|(ref_or_owned<chain_element> lhs, ref_or_owned<chain_element> rhs);

inline parsing_chain& operator|=(parsing_chain& lhs, ref_or_owned<chain_element> rhs)
{
  lhs = std::move(lhs) | rhs;
  return lhs;
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
