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

#include "formatting_style.h"

using namespace docwire;

struct ListStyle::Implementation
{
	std::string m_list_leading_characters;
};

ListStyle::ListStyle()
{
	m_impl = NULL;
	try
	{
		m_impl = new Implementation;
	}
	catch (std::bad_alloc& ba)
	{
		if (m_impl)
			delete m_impl;
		throw;
	}
}

ListStyle::ListStyle(const ListStyle& style)
{
	m_impl = NULL;
	try
	{
		m_impl = new Implementation;
		m_impl->m_list_leading_characters = std::string(style.getPrefix());
	}
	catch (std::bad_alloc& ba)
	{
		if (m_impl)
			delete m_impl;
		throw;
	}
}

ListStyle& ListStyle::operator = (const ListStyle& style)
{
	m_impl->m_list_leading_characters = std::string(style.getPrefix());
	return *this;
}

ListStyle::~ListStyle()
{
	delete m_impl;
}

void ListStyle::setPrefix(const std::string &prefix)
{
	m_impl->m_list_leading_characters = prefix;
}

void ListStyle::setPrefix(const char *prefix)
{
	m_impl->m_list_leading_characters = std::string(prefix);
}

const char* ListStyle::getPrefix() const
{
	return m_impl->m_list_leading_characters.c_str();
}
