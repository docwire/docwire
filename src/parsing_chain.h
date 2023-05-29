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

#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include "defines.h"

namespace doctotext
{

class Importer;
class Exporter;
class Transformer;

/**
 * @brief ParsingChain class is a wrapper for all defined steps of the parsing process.
 * @code
 * auto chain = Importer(parser_manager, "test.pdf")
 *            | PlainTextExporter()
 *            | std::cout; // creates a chain of steps as a ParsingChain and starts the parsing process
 * @endcode
 */
class DllExport ParsingChain
{
public:
  /**
   * @brief Constructor New parsing process from importer and exporter
   * @param importer
   * @param exporter
   */
  ParsingChain(const Importer &importer, const Exporter &exporter);

  ParsingChain(const ParsingChain &other);

  ParsingChain(const ParsingChain &&other);

  virtual ~ParsingChain();

  ParsingChain& operator=(const ParsingChain &other);

  ParsingChain& operator=(const ParsingChain &&other);

  DllExport friend ParsingChain operator|(std::istream &input_stream, ParsingChain &&parsing_process);

  DllExport friend ParsingChain operator|(std::istream &input_stream, ParsingChain &parsing_process);

  DllExport friend ParsingChain operator|(std::istream &&input_stream, ParsingChain &&parsing_process);

  DllExport friend ParsingChain operator|(std::istream &&input_stream, ParsingChain &parsing_process);

  /**
   * @brief Adds output stream for the parsing process and starts process.
   * @return ParsingChain with new output stream.
   */
  DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, std::ostream &out_stream);

  /**
   * @brief Adds output stream for the parsing process and starts process.
   * @return ParsingChain with new output stream.
   */
  DllExport friend ParsingChain operator|(ParsingChain &parsing_process, std::ostream &out_stream);

  /**
   * @brief Adds output stream for the parsing process and starts process.
   * @return ParsingChain with new output stream.
   */
    DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, std::ostream &&out_stream);

  /**
   * @brief Adds output stream for the parsing process and starts process.
   * @return ParsingChain with new output stream.
   */
    DllExport friend ParsingChain operator|(ParsingChain &parsing_process, std::ostream &&out_stream);

  /**
   * @brief Adds transformer for the parsing process.
   * @return ParsingChain with new transformer.
   */
    DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, Transformer &&transformer);

  /**
   * @brief Adds transformer for the parsing process.
   * @return ParsingChain with new transformer.
   */
    DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, Transformer &transformer);

  /**
   * @brief Adds transformer for the parsing process.
   * @return ParsingChain with new transformer.
   */
    DllExport friend ParsingChain operator|(ParsingChain &parsing_process, Transformer &&transformer);

  /**
   * @brief Adds transformer for the parsing process.
   * @return ParsingChain with new transformer.
   */
    DllExport friend ParsingChain operator|(ParsingChain &parsing_process, Transformer &transformer);

  /**
   * @brief Sets exporter for the parsing process.
   * @return ParsingChain with new exporter.
   */
    DllExport friend ParsingChain operator|(ParsingChain &parsing_process, Exporter &&exporter);

  /**
   * @brief Sets exporter for the parsing process.
   * @return ParsingChain with new exporter.
   */
    DllExport friend ParsingChain operator|(ParsingChain &parsing_process, Exporter &exporter);

  /**
   * @brief Sets exporter for the parsing process.
   * @return ParsingChain with new exporter.
   */
    DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, Exporter &&exporter);

  /**
   * @brief Sets exporter for the parsing process.
   * @return ParsingChain with new exporter.
   */
    DllExport friend ParsingChain operator|(ParsingChain &&parsing_process, Exporter &exporter);

private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;
};

DllExport ParsingChain operator|(std::istream &input_stream, const Importer &importer);
DllExport ParsingChain operator|(std::istream &input_stream, const Importer &&importer);
DllExport ParsingChain operator|(std::istream &&input_stream, const Importer &importer);
DllExport ParsingChain operator|(std::istream &&input_stream, const Importer &&importer);
DllExport ParsingChain operator|(const Importer &importer, Exporter &&exporter);
DllExport ParsingChain operator|(const Importer &importer, Transformer &&transformer);
DllExport ParsingChain operator|(const Importer &importer, Transformer &transformer);

} // namespace doctotext

#endif //PROCESS_H
