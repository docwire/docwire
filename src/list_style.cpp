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
