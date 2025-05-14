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


#ifndef DOCWIRE_PARSER_H
#define DOCWIRE_PARSER_H

#include "chain_element.h"
#include "tags.h"

namespace docwire
{

/**
 * @brief Abstract class for all parsers
 */
class DOCWIRE_CORE_EXPORT Parser : public ChainElement
{
public:

  bool is_leaf() const override { return false; }

protected:

  /**
   * @brief Executes parsing
   */
  virtual void parse(const data_source& data, const emission_callbacks& emit_tag) = 0;

  virtual const std::vector<mime_type> supported_mime_types() = 0;

  virtual continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;
};

} // namespace docwire
#endif //DOCWIRE_PARSER_H
