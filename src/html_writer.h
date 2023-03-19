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

#ifndef HTML_WRITER_HPP
#define HTML_WRITER_HPP

#include <iostream>
#include <fstream>

#include "parser.h"
#include "writer.h"
#include "defines.h"

namespace doctotext
{
/**
 * @brief The HTMLWriter class
 *
 * This class is used to converts the parsed data from callbacks into HTML format.
 *
 * example:
 * @code
 * ODFOOXMLParser parser("test.docx");
 * HTMLWriter writer;
 * parser.onNewNode([&writer](doctotext::Info &info) {
 *      writer.write_to(info, std::cout); // convert callback to html ant writes to std::cout
 *      });
 * @endcode
 */
class DllExport HtmlWriter : public Writer
{
public:

  enum class OriginalAttributesMode
  {
    skip, ///< Do not restore original HTML attributes
    restore ///< Restore original HTML attributes
  };

  /**
   * @param original_attributes_mode set how to handle original html attributes extracted by html parser
   */
  explicit HtmlWriter(OriginalAttributesMode original_attributes_mode = OriginalAttributesMode::skip);

  HtmlWriter(const HtmlWriter& html_writer);

  /**
   * @brief Writes html header to output stream. It's necessary to call this function before writing any data
   * to get valid html document.
   * @param stream output stream
   */
  void write_header(std::ostream &stream) const override;
  /**
   * @brief Writes html footer to output stream. It's necessary to call this function after writing all data
   * to get a valid html document.
   * @param stream output stream
   */
  void write_footer(std::ostream &stream) const override;
  /**
   * @brief Converts text from callback to html format
   * @param info data from callback
   * @param stream output stream
   */
  void write_to(const doctotext::Info &info, std::ostream &stream) override;
  /**
   * @brief creates a new instance of HtmlWriter
   */
  virtual Writer* clone() const override;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> impl;
};
} // namespace doctotext

#endif //HTML_WRITER_HPP
