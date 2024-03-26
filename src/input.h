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
#include "parsing_chain.h"
#include <filesystem>
#include "tags.h"

namespace docwire
{

template<class T>
concept IStreamDerived = std::derived_from<T, std::istream>;

class DllExport InputChainElement
{
public:
  explicit InputChainElement(std::shared_ptr<std::istream> stream)
  : m_tag{tag::File{.source=stream}}
  {}

  template<IStreamDerived T>
  explicit InputChainElement(T&& stream)
    : m_tag{tag::File{.source=std::make_shared<T>(std::move(stream))}}
  {}

  explicit InputChainElement(const std::filesystem::path& path)
  : m_tag{tag::File{.source=path}}
  {}

  explicit InputChainElement(std::filesystem::path&& path)
  : m_tag{tag::File{.source=std::move(path)}}
  {}

  ParsingChain operator|(ChainElement &chainElement) const;

  ParsingChain operator|(ChainElement &&chainElement) const;

  ParsingChain operator|(ParsingChain &parsingChain);

  ParsingChain operator|(ParsingChain &&parsingChain);

  void process(ChainElement& chain_element) const;

private:
  tag::File m_tag;
};

template<IStreamDerived T, ParsingChainOrChainElement U>
ParsingChain operator|(std::shared_ptr<T> stream, U& chain_element)
{
  return InputChainElement(stream) | chain_element;
}

template<IStreamDerived T, ParsingChainOrChainElement U>
ParsingChain operator|(std::shared_ptr<T> stream, U&& chain_element)
{
  return InputChainElement(stream) | std::move(chain_element);
}

template<IStreamDerived T, ParsingChainOrChainElement U>
ParsingChain operator|(T&& stream, U& chain_element)
{
  return InputChainElement(std::move(stream)) | chain_element;
}

template<IStreamDerived T, ParsingChainOrChainElement U>
ParsingChain operator|(T&& stream, U&& chain_element)
{
  return InputChainElement(std::move(stream)) | std::move(chain_element);
}

template<ParsingChainOrChainElement U>
inline ParsingChain operator|(std::filesystem::path&& path, U& chain_element)
{
  return InputChainElement(std::move(path)) | chain_element;
}

template<ParsingChainOrChainElement U>
inline ParsingChain operator|(std::filesystem::path&& path, U&& chain_element)
{
  return InputChainElement(std::move(path)) | std::move(chain_element);
}

}
#endif //DOCWIRE_INPUT_H
