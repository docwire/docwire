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

#include "input.h"

#include "log.h"
#include "parser.h"

using namespace docwire;

ParsingChain
InputBase::operator|(ChainElement &chainElement) const
{
  return ParsingChain(*this, chainElement);
}

ParsingChain
InputBase::operator|(ChainElement &&chainElement) const
{
  return ParsingChain(*this, chainElement);
}

ParsingChain
InputBase::operator|(ParsingChain &parsingChain)
{
  parsingChain.process(*this);
  return parsingChain;
}

ParsingChain
InputBase::operator|(ParsingChain &&parsingChain)
{
  parsingChain.process(*this);
  return parsingChain;
}

void
InputBase::process(ChainElement& chain_element) const
{
  docwire_log_func();
  docwire_log_var(m_path);
  if (m_stream)
  {
    Info info(StandardTag::TAG_FILE, "", {{"stream", m_stream}});
    chain_element.process(info);
  }
  else if (!m_path.empty())
  {
    Info info(StandardTag::TAG_FILE, "", {{"path", m_path}});
    chain_element.process(info);
  }
}
