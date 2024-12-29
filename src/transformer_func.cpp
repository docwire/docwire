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
  pimpl_impl(NewNodeCallback transformer_function)
    : m_transformer_function(transformer_function)
  {}

  void transform(Info &info) const
  {
    m_transformer_function(info);
    if (!info.cancel && !info.skip)
    {
      owner().emit(info);
    }
  }

  NewNodeCallback m_transformer_function;
};

TransformerFunc::TransformerFunc(NewNodeCallback transformer_function)
  : with_pimpl<TransformerFunc>(transformer_function)
{
}

TransformerFunc::TransformerFunc(TransformerFunc&&) = default;

TransformerFunc::~TransformerFunc()
{
}

void TransformerFunc::process(Info& info)
{
  impl().transform(info);
}

} // namespace docwire
