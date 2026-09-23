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

#ifndef DOCWIRE_OUTPUT_H
#define DOCWIRE_OUTPUT_H

#include "chain_element.h"
#include <concepts>
#include <memory>
#include <ostream>
#include "parsing_chain.h"
#include <type_traits>
#include <variant>
#include <vector>
#include "data_source.h"
#include "error_tags.h"
#include "ref_or_owned.h"
#include "throw_if.h"

namespace docwire
{

template<class T>
concept OStreamDerived = std::derived_from<T, std::ostream>;

template<typename T>
concept ostream_derived_ref_qualified = OStreamDerived<std::remove_reference_t<T>>;

/**
 *  @brief output_chain_element class is responsible for saving data from parsing chain to an output stream.
 *  @code
 *  std::ifstream("file.pdf", std::ios_base::in|std::ios_base::binary) | office_formats_parser{} | plain_text_exporter() | std::cout; // Imports file.pdf and saves it to std::cout as plain text
 *  @endcode
 */
class output_chain_element : public chain_element<output_chain_element>
{
public:
  /**
   * @param out_stream output_chain_element stream. Parsing chain will be writing to this stream.
   */
  output_chain_element(ref_or_owned<std::ostream> out_stream)
    : m_out_obj{out_stream}
  {}

  output_chain_element(ref_or_owned<std::vector<message_ptr>> out_vector)
    : m_out_obj{out_vector}
  {}

  continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
  std::variant<ref_or_owned<std::ostream>, ref_or_owned<std::vector<message_ptr>>> m_out_obj;
};

namespace pipeline
{

template <>
struct is_leaf<output_chain_element> : std::true_type {};

} // namespace pipeline

inline continuation output_chain_element::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  if (std::holds_alternative<ref_or_owned<std::ostream>>(m_out_obj))
  {
    if (msg->is<std::exception_ptr>())
      return emit_message(std::move(msg));
    DOCWIRE_THROW_IF(!msg->is<data_source>(),
      "Only data_source elements are supported", errors::program_logic{});
    std::shared_ptr<std::istream> in_stream = msg->get<data_source>().istream();
    std::get<ref_or_owned<std::ostream>>(m_out_obj).get() << in_stream->rdbuf();
  }
  else
  {
    std::get<ref_or_owned<std::vector<message_ptr>>>(m_out_obj).get().push_back(std::move(msg));
  }
  return continuation::proceed;
}

template <typename ChainElement>
    requires std::derived_from<std::remove_cvref_t<ChainElement>, chain_element<std::remove_cvref_t<ChainElement>>>
auto operator|(ChainElement&& element, ref_or_owned<std::ostream> stream)
{
  return std::forward<ChainElement>(element) | output_chain_element(stream);
}

template <typename ChainElement>
    requires std::derived_from<std::remove_cvref_t<ChainElement>, chain_element<std::remove_cvref_t<ChainElement>>>
auto operator|(ChainElement&& element, ref_or_owned<std::vector<message_ptr>> vector)
{
  return std::forward<ChainElement>(element) | output_chain_element(vector);
}

} // namespace docwire

#endif //DOCWIRE_OUTPUT_H
