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

#include "metadata.h"

using namespace docwire;

struct Metadata::Implementation
{
	DataType author_type;
	std::string author;
	DataType creation_date_type;
	tm creation_date;
	DataType last_modified_by_type;
	std::string last_modified_by;
	DataType last_modification_date_type;
	tm last_modification_date;
	DataType page_count_type;
	int page_count;
	DataType word_count_type;
	int word_count;
	std::map<std::string, Variant> m_fields;
};

Metadata::Metadata()
{
	impl = NULL;
	try
	{
		impl = new Implementation();
		impl->author_type = NONE;
		impl->last_modified_by_type = NONE;
		impl->creation_date_type = NONE;
		impl->last_modification_date_type = NONE;
		impl->page_count_type = NONE;
		impl->page_count = -1;
		impl->word_count_type = NONE;
		impl->word_count = -1;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Metadata::Metadata(const Metadata& r)
{
	impl = NULL;
	try
	{
		impl = new Implementation();
		*impl = *r.impl;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		throw;
	}
}

Metadata::~Metadata()
{
	delete impl;
}

Metadata& Metadata::operator=(const Metadata& r)
{
	*impl = *r.impl;
	return *this;
}

Metadata::DataType Metadata::authorType()
{
	return impl->author_type;
}

void Metadata::setAuthorType(DataType type)
{
	impl->author_type = type;
}

const char* Metadata::author()
{
	return impl->author.c_str();
}

void Metadata::setAuthor(const std::string& author)
{
	impl->author = author;
	impl->m_fields["author"] = author;
}

Metadata::DataType Metadata::creationDateType()
{
	return impl->creation_date_type;
}

void Metadata::setCreationDateType(DataType type)
{
	impl->creation_date_type = type;
}

const tm& Metadata::creationDate()
{
	return impl->creation_date;
}

void Metadata::setCreationDate(const tm& creation_date)
{
	impl->creation_date = creation_date;
	impl->m_fields["creation date"] = creation_date;
}

Metadata::DataType Metadata::lastModifiedByType()
{
	return impl->last_modified_by_type;
}

void Metadata::setLastModifiedByType(DataType type)
{
	impl->last_modified_by_type = type;
}

const char* Metadata::lastModifiedBy()
{
	return impl->last_modified_by.c_str();
}

void Metadata::setLastModifiedBy(const std::string& last_modified_by)
{
	impl->last_modified_by = last_modified_by;
	impl->m_fields["last modified by"] = last_modified_by;
}

Metadata::DataType Metadata::lastModificationDateType()
{
	return impl->last_modification_date_type;
}

void Metadata::setLastModificationDateType(DataType type)
{
	impl->last_modification_date_type = type;
}

const tm& Metadata::lastModificationDate()
{
	return impl->last_modification_date;
}

void Metadata::setLastModificationDate(const tm& last_modification_date)
{
	impl->last_modification_date = last_modification_date;
	impl->m_fields["last modification date"] = last_modification_date;
}

Metadata::DataType Metadata::pageCountType()
{
	return impl->page_count_type;
}

void Metadata::setPageCountType(DataType type)
{
	impl->page_count_type = type;
}

int Metadata::pageCount()
{
	return impl->page_count;
}

void Metadata::setPageCount(int page_count)
{
	impl->page_count = page_count;
	impl->m_fields["page count"] = (size_t)page_count;
}

Metadata::DataType Metadata::wordCountType()
{
	return impl->word_count_type;
}

void Metadata::setWordCountType(DataType type)
{
	impl->word_count_type = type;
}

int Metadata::wordCount()
{
	return impl->word_count;
}

void Metadata::setWordCount(int word_count)
{
	impl->word_count = word_count;
	impl->m_fields["word count"] = (size_t)word_count;
}

void Metadata::addField(const std::string& field_name, const Variant& field_value)
{
	impl->m_fields[field_name] = field_value;
}

bool Metadata::hasField(const std::string& field_name) const
{
	return impl->m_fields.find(field_name) != impl->m_fields.end();
}

const Variant& Metadata::getField(const std::string& field_name) const
{
	return impl->m_fields[field_name];
}

const std::map<std::string, Variant>& Metadata::getFields() const
{
	return impl->m_fields;
}

const std::map<std::string, std::any> Metadata::getFieldsAsAny() const
{
  std::map<std::string, std::any> fields;
  for (auto& field : impl->m_fields)
  {
    if (field.second.isString())
    {
      fields[field.first] = field.second.getString();
    }
    else if (field.second.isNumber())
    {
      fields[field.first] = field.second.getNumber();
    }
    else if (field.second.isDateTime())
    {
      fields[field.first] = field.second.getDateTime();
    }
  }
  return fields;
}
