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

#include <memory>

#include "chain_element.h"
#include "parser.h"
#include "defines.h"

namespace docwire
{

/**
 * @brief Wraps single function (NewNodeCallback) into ChainElement object
 * @code
 * auto reverse_text = [](Info &info) {
 *   std::reverse(info.plain_text.begin(), info.plain_text.end())}; // create function to reverse text in callback
 * TransformerFunc transformer(reverse_text); // wraps into ChainElement
 * Input("test.pdf") | ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>(parameters) | transformer | PlainTextExporter | std::cout; // reverse text in pdf file
 * @endcode
 */
class DllExport TransformerFunc : public ChainElement
{
public:
  /**
   * @param transformer_function callback function, which will be called in transform(). It should modify info structure.
   * @see Info
   */
  TransformerFunc(NewNodeCallback transformer_function);

  TransformerFunc(const TransformerFunc &other);

  virtual ~TransformerFunc();

  /**
   * @brief Executes transform operation for given node data.
   * @see Info
   * @param info
   */
  void process(Info &info) const override;

  bool is_leaf() const override
  {
    return false;
  }

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

} // namespace docwire

#endif //DOCWIRE_TRANSFORMER_FUNC_H
