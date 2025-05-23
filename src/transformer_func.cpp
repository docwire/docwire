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

#include "transformer_func.h"

namespace docwire
{

template<>
struct pimpl_impl<TransformerFunc> : with_pimpl_owner<TransformerFunc>
{
  pimpl_impl(TransformerFunc& owner, tag_transform_func transformer_function)
    : with_pimpl_owner{owner}, m_transformer_function(transformer_function)
  {}

  continuation transform(Tag&& tag, const emission_callbacks& emit_tag) const
  {
    return m_transformer_function(std::move(tag), emit_tag);
  }

  tag_transform_func m_transformer_function;
};

TransformerFunc::TransformerFunc(tag_transform_func transformer_function)
  : with_pimpl<TransformerFunc>(transformer_function)
{
}

continuation TransformerFunc::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
  return impl().transform(std::move(tag), emit_tag);
}

} // namespace docwire
