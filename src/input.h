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

#ifndef DOCWIRE_INPUT_H
#define DOCWIRE_INPUT_H

#include <iostream>
#include "chain_element.h"
#include "data_source.h"
#include "parsing_chain.h"

namespace docwire
{

template<class T>
concept IStreamDerived = std::derived_from<T, std::istream>;

template<typename T>
concept istream_derived_ref_qualified = IStreamDerived<std::remove_reference_t<T>>;

class DOCWIRE_CORE_EXPORT InputChainElement : public ChainElement
{
public:
  explicit InputChainElement(ref_or_owned<data_source> data)
    : m_data{data}
  {}

  void process(Info &info) override;
  bool is_leaf() const override { return false; }
  bool is_generator() const override { return true; }

private:
  ref_or_owned<data_source> m_data;
};

inline ParsingChain operator|(ref_or_owned<data_source> data, ref_or_owned<ChainElement> chain_element)
{
  return InputChainElement{data} | chain_element;
}

inline ParsingChain operator|(ref_or_owned<std::istream> stream, ref_or_owned<ChainElement> chain_element)
{
  return InputChainElement{data_source{seekable_stream_ptr{stream.to_shared_ptr()}}} | chain_element.to_shared_ptr();
}

template<data_source_compatible_type_ref_qualified T>
ParsingChain operator|(T&& v, ref_or_owned<ChainElement> chain_element)
{
  return data_source{std::forward<T>(v)} | chain_element;
}

}
#endif //DOCWIRE_INPUT_H
