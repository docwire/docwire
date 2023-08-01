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

#ifndef PARSER_MANAGER_HPP
#define PARSER_MANAGER_HPP

#include <optional>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include "defines.h"

namespace doctotext
{

class ParserBuilder;
class ParserProvider;

/**
 * @brief Parser manager class. Loads all available parsers and provides access to them.
 */
class DllExport ParserManager
{
public:
    ParserManager();
    /**
     * @param plugins_directory localization with plugins to be loaded
     */
    explicit ParserManager(const std::string &plugins_directory);

    ~ParserManager();
  /**
   * @brief Returns parser builder for given extension type or nullopt if no parser is found.
   * @param file_name file name with extension (e.g. ".txt", ".docx", etc.)
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByExtension(const std::string &file_name) const;

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param buffer buffer of raw data
   * @return specific parser builder or nullopt if no parser is found
   */
  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const;

  /**
   * @brief Returns all available parsers.
   * @return sets of all available parsers
   */
  std::set<std::string> getAvailableExtensions() const;


private:
  class Implementation;
  std::unique_ptr<Implementation> impl;
};

} // namespace doctotext
#endif //PARSER_MANAGER_HPP
