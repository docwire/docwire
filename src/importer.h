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

#ifndef IMPORTER_H
#define IMPORTER_H

#include <algorithm>
#include <memory>

#include "parser.h"
#include "parser_builder.h"
#include "parser_manager.h"
#include "parser_parameters.h"
#include "defines.h"

namespace doctotext
{

/**
 * @brief The Importer class. This class is used to import a file and parse it using available parsers.
 * @code
 * Importer(parser_manager, "file.pdf") | HtmlExporter() | std::cout; // Imports file.pdf and exports it to std::cout as HTML
 * @endcode
 *
 * @see Parser
 */
class DllExport Importer
{
public:
  /**
   * @param parameters parser parameters
   * @param parser_manager pointer to the parser manager
   */
  explicit Importer(const ParserParameters &parameters = ParserParameters(),
                    const std::shared_ptr<ParserManager> &parser_manager = std::make_shared<ParserManager>());
  /**
   * @param file_name name of the file to parse
   * @param parameters parser parameters
   * @param parser_manager pointer to the parser manager
   */
  Importer(const std::string &file_name,
           const ParserParameters &parameters = ParserParameters(),
           const std::shared_ptr<ParserManager> &parser_manager = std::make_shared<ParserManager>());

  /**
   *
   * @param input_stream input stream to parse
   * @param parameters parser parameters
   * @param parser_manager pointer to the parser manager
   */
  Importer(std::istream &input_stream,
           const ParserParameters &parameters = ParserParameters(),
           const std::shared_ptr<ParserManager> &parser_manager = std::make_shared<ParserManager>());

  Importer(const Importer &other);

  Importer& operator=(const Importer &other);

  virtual ~Importer();

  /**
   * @brief Sets new input stream to parse
   * @param input_stream new input stream to parse
   */
  void set_input_stream(std::istream &input_stream);

  /**
   * @brief Check if Importer contains valid input data (path to file or stream).
   * @return true if valid
   */
  bool is_valid() const;

  /**
   * @brief Adds callback. Callbacks will execute when parser returns new node.
   * @param listener
   */
  void add_callback(const NewNodeCallback &callback);

  /**
   * @brief Adds parser parameters.
   * @param parameters parser parameters
   */
  void add_parameters(const ParserParameters &parameters);

  /**
   * @brief Starts parsing process.
   */
  void process() const;

  /**
   * @brief Disconnects all listeners.
   */
  void disconnect_all();

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};


} // namespace doctotext

#endif //IMPORTER_H
