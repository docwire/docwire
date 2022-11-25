/*************************************************************************************************************/
/*  DocToText - Converts DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP),                                      */
/*              OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE),                                   */
/*              ODFXML (FODP, FODS, FODT), PDF, EML, HTML, Outlook (PST, OST),                               */
/*              Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM) documents to plain text.  */
/*              Extracts metadata and annotations.                                                           */
/*                                                                                                           */
/*  Copyright (c) 2006-2022, SILVERCODERS Ltd                                                                */
/*  http://silvercoders.com                                                                                  */
/*                                                                                                           */
/*  Project homepage: http://silvercoders.com/en/products/doctotext                                          */
/*                                                                                                           */
/*  This program may be distributed and/or modified under the terms of the                                   */
/*  GNU General Public License version 2 as published by the Free Software                                   */
/*  Foundation and appearing in the file COPYING.GPL included in the                                         */
/*  packaging of this file.                                                                                  */
/*                                                                                                           */
/*  Please remember that any attempt to workaround the GNU General Public                                    */
/*  License using wrappers, pipes, client/server protocols, and so on                                        */
/*  is considered as license violation. If your program, published on license                                */
/*  other than GNU General Public License version 2, calls some part of this                                 */
/*  code directly or indirectly, you have to buy commercial license.                                         */
/*  If you do not like our point of view, simply do not use the product.                                     */
/*                                                                                                           */
/*  Licensees holding valid commercial license for this product                                              */
/*  may use this file in accordance with the license published by                                            */
/*  SILVERCODERS and appearing in the file COPYING.COM                                                       */
/*                                                                                                           */
/*  This program is distributed in the hope that it will be useful,                                          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                                           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                                     */
/*************************************************************************************************************/

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "defines.h"
#include <iostream>
#include "writer.h"
#include <vector>

namespace doctotext
{
  struct Info;

/**
 * @brief The CsvWriter class
 *
 * This class is used to converts the parsed data from callbacks into CSV format.
 *
 * example:
 * @code
 * ODFOOXMLParser parser("test.docx");
 * CsvWriter writer;
 * parser.onNewNode([&writer](doctotext::Info &info) {
 *      writer.write_to(info, std::cout); // convert callback to csv and writes to std::cout
 *      });
 * @endcode
 */
class DllExport CsvWriter : public Writer
{
private:
  bool m_in_table { false };
  std::vector<std::string> m_curr_line;
  std::string m_curr_cell;

public:
  /**
   * @brief Converts text from callback to CSV format
   * @param info data from callback
   * @param stream output stream
   */
  void write_to(const doctotext::Info &info, std::ostream &stream) override;
  /**
   * @brief creates a new instance of HtmlWriter
   */
  virtual Writer* clone() const override;
};

} // namespace doctotext

#endif //CSV_WRITER_H
