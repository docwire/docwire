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


#ifndef DOCWIRE_PARSER_BUILDER_H
#define DOCWIRE_PARSER_BUILDER_H

#include <memory>
#include "parser.h"
#include "parser_parameters.h"
#include "defines.h"

namespace docwire
{

/**
 * Abstract class to build parsers. Parser could be built from path to file or from data buffer.
 */
class DllExport ParserBuilder
{
public:
  /**
   * @brief Builds new parser object.
   * @return pointer to new parser object
   */
  virtual std::unique_ptr<Parser> build() const = 0;

  /**
   * @brief Sets parser parameters.
   * @param inParameters
   */
  virtual ParserBuilder &withParameters(const ParserParameters &inParameters) = 0;

  virtual ~ParserBuilder() = default;
};
} // namespace docwire
#endif //DOCWIRE_PARSER_BUILDER_H
