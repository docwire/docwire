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

#include "transformer_func.h"

namespace docwire
{

template<>
struct pimpl_impl<TransformerFunc> : with_pimpl_owner<TransformerFunc>
{
  pimpl_impl(TransformerFunc& owner, message_transform_func transformer_function)
    : with_pimpl_owner{owner}, m_transformer_function(transformer_function)
  {}

  continuation transform(message_ptr msg, const message_callbacks& emit_message) const
  {
    return m_transformer_function(std::move(msg), emit_message);
  }

  message_transform_func m_transformer_function;
};

TransformerFunc::TransformerFunc(message_transform_func transformer_function)
  : with_pimpl<TransformerFunc>(transformer_function)
{
}

continuation TransformerFunc::operator()(message_ptr msg, const message_callbacks& emit_message)
{
  return impl().transform(std::move(msg), emit_message);
}

} // namespace docwire
