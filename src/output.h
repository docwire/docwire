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

#ifndef DOCWIRE_OUTPUT_H
#define DOCWIRE_OUTPUT_H

#include "chain_element.h"
#include "parsing_chain.h"
#include <type_traits>

namespace docwire
{

class Writer;

template<class T>
concept OStreamDerived = std::derived_from<T, std::ostream>;

template<typename T>
concept ostream_derived_ref_qualified = OStreamDerived<std::remove_reference_t<T>>;

/**
 *  @brief OutputChainElement class is responsible for saving data from parsing chain to an output stream.
 *  @code
 *  std::ifstream("file.pdf", std::ios_base::in|std::ios_base::binary) | office_formats_parser{} | PlainTextExporter() | std::cout; // Imports file.pdf and saves it to std::cout as plain text
 *  @endcode
 */
class DllExport OutputChainElement : public ChainElement
{
public:
  /**
   * @param out_stream OutputChainElement stream. Parsing chain will be writing to this stream.
   */
  OutputChainElement(ref_or_owned<std::ostream> out_stream)
    : m_out_stream{out_stream}
  {}

  bool is_leaf() const override
  {
    return true;
  }

  void process(Info& info) override;

private:
  ref_or_owned<std::ostream> m_out_stream;
};

inline ParsingChain operator|(ref_or_owned<ChainElement> element, ref_or_owned<std::ostream> stream)
{
  return element | OutputChainElement(stream);
}

inline ParsingChain& operator|=(ParsingChain& chain, ref_or_owned<std::ostream> stream)
{
  return chain |= OutputChainElement(stream);
}

} // namespace docwire

#endif //DOCWIRE_OUTPUT_H
