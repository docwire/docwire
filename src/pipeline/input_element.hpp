/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_PIPELINE_INPUT_ELEMENT_H
#define DOCWIRE_PIPELINE_INPUT_ELEMENT_H

#include <concepts>
#include <iostream>
#include <type_traits>
#include "element_base.hpp"
#include "../data_source.h"
#include "../serialization_data_source.h" // IWYU pragma: keep
#include "../log_entry.h"
#include "../log_scope.h"
#include "chain_expression.hpp"

namespace docwire::pipeline
{

template<class T>
concept IStreamDerived = std::derived_from<T, std::istream>;

template<typename T>
concept istream_derived_ref_qualified = IStreamDerived<std::remove_reference_t<T>>;

class input_element : public source_element<input_element>
{
public:
  explicit input_element(ref_or_owned<data_source> data)
    : m_data{data}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
  ref_or_owned<data_source> m_data;
};

inline continuation input_element::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  DOCWIRE_LOG_SCOPE();
  if (msg->is<pipeline::start_processing>())
  {
    DOCWIRE_LOG_ENTRY(m_data.get());
    return emit_message(std::move(m_data.get()));
  }
  return emit_message(std::move(msg));
}

template <typename ChainElement>
    requires transformer_or_consumer<ChainElement>
auto operator|(ref_or_owned<data_source> data, ChainElement&& chain_element)
{
  return input_element{data} | std::forward<ChainElement>(chain_element);
}

template <typename ChainElement>
    requires transformer_or_consumer<ChainElement>
auto operator|(ref_or_owned<std::istream> stream, ChainElement&& chain_element)
{
  return input_element{data_source{seekable_stream_ptr{stream.to_shared_ptr()}}} | std::forward<ChainElement>(chain_element);
}

template<data_source_compatible_type_ref_qualified T, typename ChainElement>
    requires transformer_or_consumer<ChainElement>
auto operator|(T&& v, ChainElement&& chain_element)
{
  return input_element{data_source{std::forward<T>(v)}} | std::forward<ChainElement>(chain_element);
}

} // namespace docwire::pipeline

#endif //DOCWIRE_PIPELINE_INPUT_ELEMENT_H
