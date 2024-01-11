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

#include <iostream>
#include "parser_parameters.h"
namespace docwire
{
ParserParameters::ParserParameters()
{
}

ParserParameters::ParserParameters(const std::string &name, const std::any value)
{
  m_parameters.insert(std::pair<std::string, std::any>(name, value));
}

void
ParserParameters::operator+=(const ParserParameters &parameters)
{
  for (const auto &f : parameters.m_parameters)
  {
    this->m_parameters.insert(f);
  }
}
} // namespace docwire
