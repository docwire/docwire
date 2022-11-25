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

#include "csv_writer.h"
#include "parser.hpp"

namespace doctotext
{

void
CsvWriter::write_to(const doctotext::Info &info, std::ostream &stream)
{
  if (!m_in_table && info.tag_name != StandardTag::TAG_TABLE)
    return;
  if (info.tag_name == StandardTag::TAG_TABLE)
    m_in_table = true;
  else if (info.tag_name == StandardTag::TAG_CLOSE_TABLE)
    m_in_table = false;
  else if (info.tag_name == StandardTag::TAG_CLOSE_TR)
  {
    for (int i = 0; i < m_curr_line.size(); i++)
    {
      if (i > 0)
        stream << ',';
      stream << m_curr_line[i];
    }
    stream << "\r\n";
    m_curr_line.clear();
  }
  else if (info.tag_name == StandardTag::TAG_CLOSE_TD)
  {
    m_curr_line.push_back(m_curr_cell);
    m_curr_cell = "";
  }
  else if (info.tag_name == StandardTag::TAG_TEXT)
    m_curr_cell += info.plainText;
}

Writer*
CsvWriter::clone() const
{
return new CsvWriter(*this);
}

} // namespace doctotext
