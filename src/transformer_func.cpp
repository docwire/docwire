/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "importer.h"
#include "transformer_func.h"

namespace docwire
{

class TransformerFunc::Implementation
{
public:
  Implementation(NewNodeCallback transformer_function, TransformerFunc& owner)
    : m_transformer_function(transformer_function),
      m_owner(owner)
  {}

  Implementation(const Implementation &other, TransformerFunc& owner)
    : m_transformer_function(other.m_transformer_function),
      m_owner(owner)
  {}

  Implementation(const Implementation &&other, TransformerFunc& owner)
    : m_transformer_function(other.m_transformer_function),
      m_owner(owner)
  {}

  void transform(Info &info) const
  {
    m_transformer_function(info);
    if (!info.cancel && !info.skip)
    {
      m_owner.emit(info);
    }
  }

  NewNodeCallback m_transformer_function;
  TransformerFunc& m_owner;
};

TransformerFunc::TransformerFunc(NewNodeCallback transformer_function)
{
  impl = std::unique_ptr<Implementation>{new Implementation{transformer_function, *this}};
}

TransformerFunc::TransformerFunc(const TransformerFunc &other)
: impl(new Implementation{*other.impl, *this})
{
  set_parent(other.get_parent());
}

TransformerFunc::~TransformerFunc()
{
}

void TransformerFunc::process(Info &info) const
{
  impl->transform(info);
}

TransformerFunc*
TransformerFunc::clone() const
{
  return new TransformerFunc(*this);
}

} // namespace docwire
