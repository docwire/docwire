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
#include "exception.h"
#include "parsing_chain.h"

namespace docwire
{

class Writer;

template<class T>
concept OStreamDerived = std::derived_from<T, std::ostream>;

/**
 *  @brief OutputChainElement class is responsible for saving data from parsing chain to an output stream.
 *  @code
 *  std::ifstream("file.pdf", std::ios_base::in|std::ios_base::binary) | ParseDetectedFormat<OfficeFormatsParserProvider>(parameters) | PlainTextExporter() | std::cout; // Imports file.pdf and saves it to std::cout as plain text
 *  @endcode
 */
class DllExport OutputChainElement : public ChainElement
{
public:
  /**
   * @param out_stream OutputChainElement stream. Parsing chain will be writing to this stream.
   */
  OutputChainElement(std::shared_ptr<std::ostream> out_stream)
    : m_out_stream{std::move(out_stream)}
  {}

  /**
   * @param out_stream OutputChainElement stream. Parsing chain will be writing to this stream.
   */
  template<OStreamDerived T>
  OutputChainElement(T&& out_stream)
    : OutputChainElement{std::make_shared<T>(std::move(out_stream))}
  {}

  /**
   * @brief Constructs OutputChainElement with a non-owning pointer (reference) to an output stream.
   * @details This constructor is useful when you want to pass a stack variable, global variable or predefined objects like as output stream.
   * @warning It is your responsibility to ensure that the stream outlives OutputChainElement. Consider using std::shared_ptr instead.
   * @param out_stream OutputChainElement stream. Parsing chain will be writing to this stream.
   */
  template<OStreamDerived T>
  OutputChainElement(T& out_stream)
    : OutputChainElement(std::shared_ptr<T>{&out_stream, [](auto*) {}})
  {
  }

  virtual ~OutputChainElement() = default;

  bool is_leaf() const override
  {
    return true;
  }

  void process(Info &info) const override;

private:
  std::shared_ptr<std::ostream> m_out_stream;
};

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<E> element_or_chain, std::shared_ptr<S> stream)
{
  return element_or_chain | OutputChainElement(stream);
}

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(E&& element_or_chain, std::shared_ptr<S> stream)
{
  return std::move(element_or_chain) | OutputChainElement(stream);
}

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<E> element_or_chain, S&& stream)
{
  return element_or_chain | OutputChainElement(std::move(stream));
}

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(E&& element_or_chain, S&& stream)
{
  return std::move(element_or_chain) | OutputChainElement(std::move(stream));
}

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(std::shared_ptr<E> element_or_chain, S& stream)
{
  return element_or_chain | OutputChainElement(stream);
}

template<ParsingChainOrChainElement E, OStreamDerived S>
std::shared_ptr<ParsingChain> operator|(E&& element_or_chain, S& stream)
{
  return std::move(element_or_chain) | OutputChainElement(stream);
}

} // namespace docwire

#endif //DOCWIRE_OUTPUT_H
