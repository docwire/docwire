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

class DllExport InputChainElement
{
public:
  explicit InputChainElement(data_source&& data)
    : m_data{std::move(data)}
  {}

	template <data_source_compatible_type T>
	explicit InputChainElement(const T& v)
		: m_data{data_source{v}}
	{}

	template <data_source_compatible_type T>
	explicit InputChainElement(T&& v)
		: m_data{data_source{std::move(v)}}
	{}

  explicit InputChainElement(std::shared_ptr<std::istream> stream)
  : m_data{seekable_stream_ptr{stream}}
  {}

  template<istream_derived_ref_qualified T>
  explicit InputChainElement(T&& stream)
    : m_data{seekable_stream_ptr{std::make_shared<T>(std::forward<T>(stream))}}
  {}

  void process(ChainElement& chain_element) const;

private:
  data_source m_data;
};

inline std::shared_ptr<ParsingChain> operator|(InputChainElement&& input, std::shared_ptr<ParsingChain> parsingChain)
{
  parsingChain->process(input);
  return parsingChain;
}

template<ChainElementDerived T>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<InputChainElement> input, std::shared_ptr<T> chainElement)
{
  return std::make_shared<ParsingChain>(input, chainElement);
}

template<chain_element_derived_ref_qualified T>
std::shared_ptr<ParsingChain> operator|(InputChainElement&& input, T&& chainElement)
{
  return std::make_shared<ParsingChain>(std::make_shared<InputChainElement>(std::move(input)), std::make_shared<T>(std::forward<T>(chainElement)));
}

template<ParsingChainOrChainElement U>
std::shared_ptr<ParsingChain> operator|(data_source&& data, std::shared_ptr<U> chain_element)
{
  return InputChainElement{std::move(data)} | chain_element;
}

template<parsing_chain_or_chain_element_ref_qualified U>
std::shared_ptr<ParsingChain> operator|(data_source&& data, U&& chain_element)
{
  return InputChainElement(std::move(data)) | std::forward<U>(chain_element);
}

template<IStreamDerived T, ParsingChainOrChainElement U>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<T> stream, std::shared_ptr<U> chain_element)
{
  return InputChainElement(stream) | chain_element;
}

template<IStreamDerived T, parsing_chain_or_chain_element_ref_qualified U>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<T> stream, U&& chain_element)
{
  return InputChainElement(stream) | std::forward<U>(chain_element);
}

template<istream_derived_ref_qualified T, ParsingChainOrChainElement U>
std::shared_ptr<ParsingChain> operator|(T&& stream, std::shared_ptr<U> chain_element)
{
  return InputChainElement(std::forward<T>(stream)) | chain_element;
}

template<istream_derived_ref_qualified T, parsing_chain_or_chain_element_ref_qualified U>
std::shared_ptr<ParsingChain> operator|(T&& stream, U&& chain_element)
{
  return InputChainElement(std::forward<T>(stream)) | std::forward<U>(chain_element);
}

template<data_source_compatible_type_ref_qualified T, ParsingChainOrChainElement U>
inline std::shared_ptr<ParsingChain> operator|(T&& v, std::shared_ptr<U> chain_element)
{
  return InputChainElement(std::forward<T>(v)) | chain_element;
}

template<data_source_compatible_type_ref_qualified T, ParsingChainOrChainElement U>
inline std::shared_ptr<ParsingChain> operator|(T&& v, U&& chain_element)
{
  return InputChainElement(std::forward<T>(v)) | std::forward<U>(chain_element);
}

}
#endif //DOCWIRE_INPUT_H
