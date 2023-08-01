/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#ifndef PARSER_PARAMETERS_HPP
#define PARSER_PARAMETERS_HPP

#include <iostream>
#include <optional>
#include <any>
#include <map>
#include <string>
#include "defines.h"

namespace doctotext
{
/**
 * @brief Stores list of parsers parameters. Every parser can query ParserParameter for a specific parameter.
 * For example OCRParser queries ParserParameters for a language. Every parser contains ParserParameters and
 * recursively passes it to another parser.
 *
 * Example:
 * @code
 * ParserParameters parameters("language", doctotext::Language::english); // Important: explicit type
 * parameters += ParserParameters("TESSDATA_PREFIX", "tessdata"); // Adds second parameter
 *
 * Importer importer("img.jpg");
 * importer.add_parameters(parameters)
 *
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
} // namespace doctotext

#endif //PARSER_PARAMETERS_HPP
