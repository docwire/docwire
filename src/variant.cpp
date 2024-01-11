/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

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
