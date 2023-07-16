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

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <algorithm>
#include <memory>

#include "chain_element.h"
#include "parser.h"
#include "parser_builder.h"
#include "parser_manager.h"
#include "parser_parameters.h"
#include "defines.h"

namespace doctotext
{
class Importer;

/**
 * @brief Wraps single function (doctotext::NewNodeCallback) into ChainElement object
 * @code
 * auto reverse_text = [](doctotext::Info &info) {
 *   std::reverse(info.plain_text.begin(), info.plain_text.end())}; // create function to reverse text in callback
 * TransformerFunc transformer(reverse_text); // wraps into ChainElement
 * Importer(parser_manager, "test.pdf") | transformer | PlainTextExporter | std::cout; // reverse text in pdf file
 * @endcode
 */
class DllExport TransformerFunc : public ChainElement
{
public:
  /**
   * @param transformer_function callback function, which will be called in transform(). It should modify info structure.
   * @see doctotext::Info
   */
  TransformerFunc(doctotext::NewNodeCallback transformer_function);

  TransformerFunc(const TransformerFunc &other);

  virtual ~TransformerFunc();

  /**
   * @brief Executes transform operation for given node data.
   * @see doctotext::Info
   * @param info
   */
  void process(doctotext::Info &info) const;

  bool is_leaf() const override
  {
    return false;
  }

  /**
   * @brief Creates clone of the TransformerFunc
   * @return new TransformerFunc
   */
  TransformerFunc* clone() const override;

private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

} // namespace doctotext

#endif //TRANSFORMER_H
