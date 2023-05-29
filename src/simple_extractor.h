/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
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

#ifndef SIMPLE_EXTRACTOR_HPP
#define SIMPLE_EXTRACTOR_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <sstream>

#include "importer.h"
#include "exporter.h"
#include "transformer.h"
#include "parsing_chain.h"

#include "formatting_style.h"
#include "defines.h"

namespace doctotext
{

/**
 * @brief The SimpleExtractor class provides basic functionality for extracting text from a document.
 * @code
 * SimpleExtractor extractor("test.docx");
 * std::string plain_text = extractor.getPlainText(); // get the plain text from the document
 * std::string html = extractor.getHtmlText(); // get the text as a html from the document
 * std::string metadata = extractor.getMetadata(); // get the metadata as a plain text from the document
 * @endcode
 */
class DllExport SimpleExtractor
{
public:
  /**
   * @param file_name name of the file to parse
   */
  explicit SimpleExtractor(const std::string &file_name, const std::string &plugins_path = "./plugins");

  /**
   * @param input_stream input stream to parse
   */
  SimpleExtractor(std::istream &input_stream, const std::string &plugins_path = "./plugins");

  ~SimpleExtractor();

  /**
   * @brief Extracts the text from the file.
   * @return parsed file as plain text
   */
  std::string getPlainText() const;

  /**
   * @brief Extracts the data from the file and converts it to the html format.
   * @return parsed file ashtml text
   */
  std::string getHtmlText() const;

  void parseAsPlainText(std::ostream &out_stream) const;

  void parseAsHtml(std::ostream &out_stream) const;

  /**
   * @brief Extracts the meta data from the file.
   * @return parsed meta data as plain text
   */
  std::string getMetaData() const;

  /**
   * @brief Sets the formatting style.
   * @param style
   */
  void setFormattingStyle(const FormattingStyle &style);

  /**
   * @brief Adds callback function to the extractor.
   * @code
   * extractor.addCallbackFunction(StandardFilter::filterByMailMaxCreationTime(creation_time));
   * @brief
   * @param filter
   */
  void addCallbackFunction(NewNodeCallback new_code_callback);

  /**
   * @brief Adds parser parameters.
   * @param parameters
   */
  void addParameters(const ParserParameters &parameters);

  /**
   * @brief Adds transformer.
   * @code
   * extractor.addTransformer(new UpperTextTransformer());
   * @endcode
   * @param transformer as a raw pointer. The ownership is transferred to the extractor.
   */
  void addTransformer(Transformer *transformer);

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};


} // namespace doctotext


#endif //SIMPLE_EXTRACTOR_HPP
