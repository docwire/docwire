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

#ifndef DOCWIRE_TRANSFORMER_FUNC_H
#define DOCWIRE_TRANSFORMER_FUNC_H

#include "chain_element.h"
#include "core_export.h"
#include "parsing_chain.h"
#include "ref_or_owned.h"

namespace docwire
{

using message_transform_func = std::function<continuation(message_ptr, const message_callbacks& emit_message)>;

/**
 * @brief Wraps single function (tag_transform_func) into chain_element object
 */
class DOCWIRE_CORE_EXPORT transformer_func : public chain_element, public with_pimpl<transformer_func>
{
public:
  /**
   * @param transformer_function callback function, which will be called in transform().
   */
  transformer_func(message_transform_func transformer_function);

	/**
	 * @brief Executes transform on the given message.
	 * @see docwire::message_ptr
	 * @param msg Incoming message.
	 * @param emit_message Callback to emit downstream messages.
	 */
	virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

  bool is_leaf() const override
  {
    return false;
  }

private:
  using with_pimpl<transformer_func>::impl;
  friend pimpl_impl<transformer_func>;
};

template <typename T>
requires (
  std::is_convertible_v<T, message_transform_func> &&
  !std::is_base_of_v<chain_element, std::remove_cvref_t<T>>
)
parsing_chain operator|(ref_or_owned<chain_element> element, T func)
{
  return element | transformer_func{func};
}

template <typename T>
requires (
  std::is_convertible_v<T, message_transform_func> &&
  !std::is_base_of_v<chain_element, std::remove_cvref_t<T>>
)
parsing_chain& operator|=(parsing_chain& chain, T func)
{
  return chain |= transformer_func{func};
}

} // namespace docwire

#endif //DOCWIRE_TRANSFORMER_FUNC_H
