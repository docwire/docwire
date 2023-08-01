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

#include "attachment.h"

using namespace doctotext;

struct Attachment::Implementation
{
	std::string m_file_name;
	std::string m_binary_content;
	std::map<std::string, Variant> m_fields;
};

Attachment::Attachment()
{
	impl = NULL;
	try
	{
		impl = new Implementation;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		impl = NULL;
		throw;
	}
}

Attachment::Attachment(const std::string &file_name)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_file_name = file_name;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		impl = NULL;
		throw;
	}
}

Attachment::Attachment(const Attachment &attachment)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		*impl = *attachment.impl;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		impl = NULL;
		throw;
	}
}

Attachment& Attachment::operator = (const Attachment& attachment)
{
	*impl = *attachment.impl;
	return *this;
}

Attachment::~Attachment()
{
	if (impl)
		delete impl;
}

void Attachment::setFileName(const std::string& file_name)
{
	impl->m_file_name = file_name;
}

void Attachment::setBinaryContent(const std::string &binary_content)
{
	impl->m_binary_content = binary_content;
}

void Attachment::addField(const std::string &field_name, const Variant &field_value)
{
	impl->m_fields[field_name] = field_value;
}

const char* Attachment::filename() const
{
	return impl->m_file_name.c_str();
}

const char* Attachment::binaryContent() const
{
	return impl->m_binary_content.c_str();
}

size_t Attachment::binaryContentSize() const
{
	return impl->m_binary_content.length();
}

bool Attachment::hasField(const std::string& field_name) const
{
	return impl->m_fields.find(field_name) != impl->m_fields.end();
}

const Variant& Attachment::getField(const std::string& field_name) const
{
	return impl->m_fields[field_name];
}

const std::map<std::string, Variant>& Attachment::getFields() const
{
	return impl->m_fields;
}
