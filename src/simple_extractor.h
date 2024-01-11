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

#ifndef DOCWIRE_SIMPLE_EXTRACTOR_H
#define DOCWIRE_SIMPLE_EXTRACTOR_H

#include "parser.h"

namespace docwire
{

class ChainElement;

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
  explicit SimpleExtractor(const std::string &file_name, const std::string &plugins_path = "");

  /**
   * @param input_stream input stream to parse
   */
  SimpleExtractor(std::istream &input_stream, const std::string &plugins_path = "");

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

  void parseAsCsv(std::ostream &out_stream) const;

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
  void addCallbackFunction(const NewNodeCallback& new_code_callback);

  /**
   * @brief Adds parser parameters.
   * @param parameters
   */
  void addParameters(const ParserParameters &parameters);

  /**
   * @brief Adds transformer.
   * @code
   * extractor.addChainElement(new UpperTextTransformer());
   * @endcode
   * @param transformer as a raw pointer. The ownership is transferred to the extractor.
   */
  void addChainElement(ChainElement *chainElement);

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};


} // namespace docwire


#endif //DOCWIRE_SIMPLE_EXTRACTOR_H
