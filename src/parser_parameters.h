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

#ifndef DOCWIRE_PARSER_PARAMETERS_H
#define DOCWIRE_PARSER_PARAMETERS_H

#include <optional>
#include <any>
#include <map>
#include <string>
#include "defines.h"

namespace docwire
{
/**
 * @brief Stores list of parsers parameters. Every parser can query ParserParameter for a specific parameter.
 * For example OCRParser queries ParserParameters for a language.
 *
 * Example:
 * @code
 * ParserParameters parameters("languages", std::vector<Language>(Language::eng));
 * std::filesystem::path{"img.jpg"} | ParseDetectedFormat<OCRParserProvider>(parameters) | PlainTextExporter() | std::cout;
 * @endcode
 */
class DllExport ParserParameters
{
public:
  /**
   * @brief Creates empty ParserParameters.
   */
  ParserParameters();

  ParserParameters(const ParserParameters& parameters)
  {
    for (auto parameter : parameters.m_parameters)
    {
      m_parameters.insert(parameter);
    }
  }

  /**
   * Creates ParserParameters with one defined parameter.
   * @param name name of parameter
   * @param value value of parameter
   */
  ParserParameters(const std::string &name, const std::any value);

  /**
   * @brief Adds all parameters from the second ParserParameters.
   * @param parameters
   */
  void operator+=(const ParserParameters &parameters);

  ParserParameters& operator=(const ParserParameters &parameters)
  {
    for (auto parameter : parameters.m_parameters)
    {
      m_parameters.insert(parameter);
    }
    return *this;
  }

  /**
   * @brief Returns wanted parameter if it exists.
   * @param name name of wanted parameter
   * @return returns found parameter, if parameter doesn't exist returns empty optional
   */
  template<typename T>
  std::optional<T> getParameterValue(const std::string &name) const
  {
    auto parameter_value = m_parameters.find(name);
    if (parameter_value!= m_parameters.end() && parameter_value->second.type() == typeid(T))
    {
      return std::any_cast<T>(parameter_value->second);
    }
    return std::nullopt;
  }

private:
  std::map<std::string, std::any> m_parameters;
};
} // namespace docwire

#endif //DOCWIRE_PARSER_PARAMETERS_H
