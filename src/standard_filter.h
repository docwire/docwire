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

#ifndef STANDARD_FILTER_H
#define STANDARD_FILTER_H

#include <algorithm>
#include "parser.h"
#include "defines.h"

namespace doctotext
{
/**
 * @brief Sets of standard filters to use in parsers.
 * example of use:
 * @code
 *  PSTParser pst_parser("test.pst");
 *  pst_parser.onNewNode(StandardFilter::filterByFolderName({"Inbox", "Sent"}))
 *      .onNewNode(StandardFilter::filterByAttachmentType({"jpg", "png"}))
 *      .parse();
 * @endcode
 */
class DllExport StandardFilter
{
public:
  /**
   * @brief Filters folders by name. Keeps only folders with names that exist in the given list.
   * @param names list of names to keep
   */
  static doctotext::NewNodeCallback filterByFolderName(const std::vector<std::string> &names);

  /**
   * @brief Filters attachments by type. Keeps only attachments with type that exist in the given list.
   * @param types list of types to keep
   */
  static doctotext::NewNodeCallback filterByAttachmentType(const std::vector<std::string> &types);

  /**
   * @brief Filters mail by creation date. Keeps only mails that are created after the given date.
   * @param min_time minimum time to keep
   */
  static doctotext::NewNodeCallback filterByMailMinCreationTime(unsigned int min_time);

  /**
   * @brief Filters mail by creation date. Keeps only mails that are created before the given date.
   * @param max_time maximum time to keep
   */
  static doctotext::NewNodeCallback filterByMailMaxCreationTime(unsigned int max_time);

  /**
   * @brief
   * @param max_nodes
   */
  static doctotext::NewNodeCallback filterByMaxNodeNumber(unsigned int max_nodes);
};
} // namespace doctotext

#endif //STANDARD_FILTER_H
