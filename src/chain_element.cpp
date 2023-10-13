/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
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
/*  https://github.com/docwire/docwire                                                                                                             */
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

#include <boost/signals2.hpp>

#include "chain_element.h"
#include "parsing_chain.h"

namespace docwire
{

struct ChainElement::Implementation
{
  Implementation()
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(Info &info)>>())
  {}

  Implementation(const Implementation& implementation)
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(Info &info)>>())
  {}

  void connect(const ChainElement &chain_element)
  {
    m_on_new_node_signal->connect([&chain_element](Info &info){chain_element.process(info);});
  }

  void emit(Info &info) const
  {
    (*m_on_new_node_signal)(info);
  }

  std::shared_ptr<boost::signals2::signal<void(Info &info)>> m_on_new_node_signal;
};

ChainElement::ChainElement()
{
  base_impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{}, ImplementationDeleter{}};
}

ChainElement::ChainElement(const ChainElement& element)
: base_impl(new Implementation(*(element.base_impl))),
  m_parent(element.m_parent)
{}

ChainElement&
ChainElement::operator=(const ChainElement &chain_element)
{
  base_impl->m_on_new_node_signal = chain_element.base_impl->m_on_new_node_signal;
  m_parent = chain_element.m_parent;
  return *this;
}

ParsingChain
ChainElement::operator|(ChainElement& chainElement)
{
  return ParsingChain(*this, chainElement);
}

ParsingChain
ChainElement::operator|(ChainElement&& chainElement)
{
  return ParsingChain(*this, chainElement);
}

void
ChainElement::connect(const ChainElement &chain_element)
{
  base_impl->connect(chain_element);
}

void
ChainElement::set_parent(const std::shared_ptr<ChainElement>& chainElement)
{
  m_parent = chainElement;
}

std::shared_ptr<ChainElement>
ChainElement::get_parent() const
{
  return m_parent;
}

void
ChainElement::emit(Info &info) const
{
  base_impl->emit(info);
}

void
ChainElement::ImplementationDeleter::operator()(ChainElement::Implementation *impl)
{
  delete impl;
}

} // namespace docwire
