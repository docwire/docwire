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
 * @brief Wraps single function (tag_transform_func) into ChainElement object
 */
class DOCWIRE_CORE_EXPORT TransformerFunc : public ChainElement, public with_pimpl<TransformerFunc>
{
public:
  /**
   * @param transformer_function callback function, which will be called in transform().
   */
  TransformerFunc(message_transform_func transformer_function);

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
  using with_pimpl<TransformerFunc>::impl;
  friend pimpl_impl<TransformerFunc>;
};

template <typename T>
requires (
  std::is_convertible_v<T, message_transform_func> &&
  !std::is_base_of_v<ChainElement, std::remove_cvref_t<T>>
)
ParsingChain operator|(ref_or_owned<ChainElement> element, T func)
{
  return element | TransformerFunc{func};
}

template <typename T>
requires (
  std::is_convertible_v<T, message_transform_func> &&
  !std::is_base_of_v<ChainElement, std::remove_cvref_t<T>>
)
ParsingChain& operator|=(ParsingChain& chain, T func)
{
  return chain |= TransformerFunc{func};
}

} // namespace docwire

#endif //DOCWIRE_TRANSFORMER_FUNC_H
