/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_PIPELINE_INPUT_ELEMENT_H
#define DF0_PIPELINE_INPUT_ELEMENT_H

#include <concepts>
#include <iostream>
#include <type_traits>
#include "element_base.hpp"
#include "../data_source.h"
#include "../serialization_data_source.h" // IWYU pragma: keep
#include "../audit/auditor_ref.hpp"
#include "../audit/scope.hpp"
#include "../audit/entry.hpp"
#include "chain_expression.hpp"

namespace df0::pipeline
{

template<class T>
concept IStreamDerived = std::derived_from<T, std::istream>;

template<typename T>
concept istream_derived_ref_qualified = IStreamDerived<std::remove_reference_t<T>>;

/**
 * @brief The standard pipeline source backed by a `data_source`.
 *
 * The first call is expected to carry `pipeline::start_processing`;
 * `input_element` then emits the wrapped `data_source` downstream.
 *
 * Later back-emitted messages are forwarded downstream unchanged so that the
 * fully assembled pipeline can process them again.
 *
 * @note A back-emitted message must not be `pipeline::start_processing`.
 *       See `message_callbacks::back`.
 */
class input_element : public source_element<input_element>
{
public:
  explicit input_element(ref_or_owned<data_source> data)
    : m_data{data}
  {}

  template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
  continuation operator()(message_ptr msg, const message_callbacks& emit_message)
  {
      return operator()<stateless_auditor_type>(
          std::move(msg),
          emit_message,
          audit::auditor_ref<stateless_auditor_type>{});
  }

  template<typename auditor_type>
  continuation operator()(message_ptr msg,
                          const message_callbacks& emit_message,
                          audit::auditor_ref<auditor_type> auditor);

private:
  ref_or_owned<data_source> m_data;
};

template<typename auditor_type>
inline continuation input_element::operator()(message_ptr msg, const message_callbacks& emit_message,
                                              audit::auditor_ref<auditor_type> auditor)
{
  audit::scope audit_scope{auditor, audit::no_context};
  if (msg->is<pipeline::start_processing>())
  {
    audit::entry(auditor, [&]{ return "data"_v = m_data.get(); });
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

} // namespace df0::pipeline

#endif //DF0_PIPELINE_INPUT_ELEMENT_H
