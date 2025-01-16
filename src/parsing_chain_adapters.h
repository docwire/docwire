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

#ifndef DOCWIRE_PARSING_CHAIN_ADAPTERS_H
#define DOCWIRE_PARSING_CHAIN_ADAPTERS_H

#include "chain_element.h"
#include "data_source.h"
#include "error_tags.h"
#include <functional>
#include "parsing_chain.h"
#include "ref_or_owned.h"
#include "throw_if.h"
#include "transformer_func.h"

namespace docwire
{

inline ParsingChain operator|(ref_or_owned<ChainElement> element, NewNodeCallback func)
{
  return element | TransformerFunc{func};
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_ADAPTERS_H
