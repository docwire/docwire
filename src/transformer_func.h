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

#ifndef DOCWIRE_TRANSFORMER_FUNC_H
#define DOCWIRE_TRANSFORMER_FUNC_H

#include "chain_element.h"
#include "parser.h"
#include "defines.h"
#include "parsing_chain.h"
#include "ref_or_owned.h"

namespace docwire
{

typedef std::function<void(Info &info)> NewNodeCallback;

/**
 * @brief Wraps single function (NewNodeCallback) into ChainElement object
 * @code
 * auto reverse_text = [](Info &info) {
 *   std::reverse(info.plain_text.begin(), info.plain_text.end())}; // create function to reverse text in callback
 * TransformerFunc transformer(reverse_text); // wraps into ChainElement
 * Input("test.pdf") | office_formats_parser{} | transformer | PlainTextExporter | std::cout; // reverse text in pdf file
 * @endcode
 */
class DllExport TransformerFunc : public ChainElement, public with_pimpl<TransformerFunc>
{
public:
  /**
   * @param transformer_function callback function, which will be called in transform(). It should modify info structure.
   * @see Info
   */
  TransformerFunc(NewNodeCallback transformer_function);

  /**
   * @brief Executes transform operation for given node data.
   * @see Info
   * @param info
   */
  void process(Info& info) override;

  bool is_leaf() const override
  {
    return false;
  }

private:
  using with_pimpl<TransformerFunc>::impl;
  friend pimpl_impl<TransformerFunc>;
};

inline ParsingChain operator|(ref_or_owned<ChainElement> element, NewNodeCallback func)
{
  return element | TransformerFunc{func};
}

inline ParsingChain& operator|=(ParsingChain& chain, NewNodeCallback func)
{
  return chain |= TransformerFunc{func};
}

} // namespace docwire

#endif //DOCWIRE_TRANSFORMER_FUNC_H
