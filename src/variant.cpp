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

#include "variant.h"

#include "misc.h"
#include <string.h>

using namespace docwire;

struct Variant::Implementation
{
	enum VariantType
	{
		string,
		number,
		date_time,
		null
	};

	std::string m_string_value;
	size_t m_number_value;
	tm m_date_time_value;
	VariantType m_variant_type;

	Implementation()
	{
		m_variant_type = null;
		m_number_value = 0;
		memset(&m_date_time_value, 0, sizeof(m_date_time_value));
	}
};

Variant::Variant()
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
		throw;
	}
}

Variant::Variant(const std::string& value)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		setValue(value);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Variant::Variant(const tm& value)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		setValue(value);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Variant::Variant(size_t value)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		setValue(value);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Variant::Variant(const Variant& variant)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		*impl = *variant.impl;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Variant& Variant::operator = (const Variant& variant)
{
	*impl = *variant.impl;
	return *this;
}

Variant::~Variant()
{
	if (impl)
		delete impl;
}

void Variant::setValue(size_t number)
{
	impl->m_variant_type = Implementation::number;
	impl->m_number_value = number;
	impl->m_string_value = uint_to_string(number);
}

void Variant::setValue(const tm& date_time)
{
	impl->m_variant_type = Implementation::date_time;
	impl->m_date_time_value = date_time;
	impl->m_string_value = date_to_string(impl->m_date_time_value);
}

void Variant::setValue(const std::string& str)
{
	impl->m_variant_type = Implementation::string;
	impl->m_string_value = str;
}

bool Variant::isEmpty() const
{
	return impl->m_variant_type == Implementation::null;
}

bool Variant::isString() const
{
	return impl->m_variant_type == Implementation::string;
}

bool Variant::isNumber() const
{
	return impl->m_variant_type == Implementation::number;
}

bool Variant::isDateTime() const
{
	return impl->m_variant_type == Implementation::date_time;
}

const tm& Variant::getDateTime() const
{
	return impl->m_date_time_value;
}

const char* Variant::getString() const
{
	return impl->m_string_value.c_str();
}

size_t Variant::getNumber() const
{
	return impl->m_number_value;
}
