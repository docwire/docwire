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

#ifndef DOCWIRE_PARSER_WRAPPER_H
#define DOCWIRE_PARSER_WRAPPER_H

#include "parser.h"
#include "parser_builder.h"
#include "defines.h"

namespace docwire
{

/**
 * @brief Provides the basic mechanism to build any parser.
 * @tparam ParserType type of parser to build
 */
template<typename ParserType>
class DllExport ParserBuilderWrapper : public ParserBuilder
{
public:
  ParserBuilderWrapper()
  {

  }

  std::unique_ptr<Parser>
  build() const override
  {
    auto parser = std::make_unique<ParserType>();
    parser->withParameters(m_parameters);
    return parser;
  }

  ParserBuilder& withParameters(const ParserParameters &inParameter) override
  {
    m_parameters += inParameter;
    return *this;
  }

private:
  ParserParameters m_parameters;
};
} // namespace docwire
#endif //DOCWIRE_PARSER_WRAPPER_H
