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


#ifndef DOCWIRE_PST_PARSER_H
#define DOCWIRE_PST_PARSER_H

#include "mail_export.h"
#include <vector>

#include "chain_element.h"
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_MAIL_EXPORT PSTParser : public ChainElement, public with_pimpl<PSTParser>
{
private:
  using with_pimpl<PSTParser>::impl;
  friend pimpl_impl<PSTParser>;

public:
  PSTParser();
  continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;
  bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif //DOCWIRE_PST_PARSER_H
