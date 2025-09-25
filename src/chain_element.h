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

#include "core_export.h"
#include "pimpl.h"
#include "message.h"

namespace docwire
{

class ParsingChain;

class DOCWIRE_CORE_EXPORT ChainElement : public with_pimpl<ChainElement>
{
public:
  ChainElement();
  ChainElement(ChainElement&&) = default;
  virtual ~ChainElement() = default;
  ChainElement& operator=(ChainElement&&) = default;

  virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) = 0;

  /**
   * @brief Check if ChainElement is a leaf (last element which doesn't produce any messages). At this moment only Exporters are leafs.
   * @return true if leaf
   */
  virtual bool is_leaf() const = 0;

  virtual bool is_generator() const { return false; }
};

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
