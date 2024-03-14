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

#include <memory>
#include <iostream>
#include "defines.h"
#include <vector>

namespace docwire
{

class InputBase;

class ChainElement;

  class DllExport ParsingChain
  {
  public:
    explicit ParsingChain(const ChainElement& element);
    ParsingChain(ChainElement& element1, ChainElement& element2);
    ParsingChain(const InputBase &input, ChainElement& element);
    ParsingChain& operator|(const ChainElement& element);
    ParsingChain& operator|(ChainElement&& element);

    void process(InputBase& input);
  private:
    const InputBase* m_input;
    std::shared_ptr<ChainElement> first_element;
    std::shared_ptr<ChainElement> last_element;
    std::vector<std::shared_ptr<ChainElement>> element_list;
  };

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_H
