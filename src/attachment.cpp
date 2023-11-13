/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "attachment.h"

using namespace docwire;

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
