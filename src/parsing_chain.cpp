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

#include "input.h"
#include "parsing_chain.h"

namespace docwire
{
  ParsingChain::ParsingChain(ChainElement& element1, ChainElement& element2)
  : first_element(std::shared_ptr<ChainElement>(element1.clone())),
    last_element(std::shared_ptr<ChainElement>(element2.clone())),
    m_input(nullptr)
  {
    first_element->connect(*last_element);
    last_element->set_parent(first_element);
    element_list = {first_element, last_element};
  }

  ParsingChain::ParsingChain(const ChainElement& element)
  : first_element(std::shared_ptr<ChainElement>(element.clone())),
    m_input(nullptr)
  {
    element_list = {first_element};
  }

  ParsingChain::ParsingChain(const InputBase &input, ChainElement& element)
  : m_input(&input),
    first_element(std::shared_ptr<ChainElement>(element.clone()))
  {
    element_list = {first_element};
  }

  ParsingChain&
  ParsingChain::operator|(const ChainElement& element)
  {
    auto element_ptr = std::shared_ptr<ChainElement>(element.clone());
    element_list.push_back(element_ptr);
    if (last_element)
    {
      last_element->connect(*element_ptr);
      element_ptr->set_parent(last_element);
    }
    else
    {
      first_element->connect(*element_ptr);
      element_ptr->set_parent(first_element);
    }
    last_element = element_ptr;

    if (last_element->is_leaf())
    {
      if (m_input)
      {
        m_input->process(*first_element);
      }
    }
    return *this;
  }

  ParsingChain&
  ParsingChain::operator|(ChainElement&& element)
  {
    return operator|(element);
  }

  void
  ParsingChain::process(InputBase& input)
  {
    m_input = &input;
    if (last_element && last_element->is_leaf())
    {
      input.process(*first_element);
    }
  }

} // namespace docwire