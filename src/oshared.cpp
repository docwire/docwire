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

#include "oshared.h"

#include "exception.h"
#include "misc.h"
#include <iostream>
#include "log.h"
#include "wv2/utilities.h"
#include <time.h>
#include "thread_safe_ole_stream_reader.h"
#include "thread_safe_ole_storage.h"

namespace docwire
{

using namespace wvWare;

static bool read_vt_string(ThreadSafeOLEStreamReader* reader, std::string& s)
{
	U16 string_type;
	if (!reader->readU16(string_type) || string_type != 0x1E)
	{
		docwire_log(error) << "Incorrect string type.";
		return false;
	}
	reader->seek(2, SEEK_CUR); //padding
	U32 len;
	reader->readU32(len);
	s = "";
	for (int j = 0; j < len - 1; j++)
	{
		S8 ch;
		reader->readS8(ch);
		if (ch == '\0')
			break;
		s += ch;
	}
	if (!reader->isValid())
	{
		docwire_log(error) << "OLE Reader error message: " << reader->getLastError();
		return false;
	}
	return true;
}

static bool read_vt_i4(ThreadSafeOLEStreamReader* reader, S32& i)
{
	U16 string_type;
	if (!reader->readU16(string_type) || string_type != 0x0003)
	{
		docwire_log(error) << "Incorrect value type.";
		return false;
	}
	reader->seek(2, SEEK_CUR); //padding
	reader->readS32(i);
	if (!reader->isValid())
	{
		docwire_log(error) << reader->getLastError();
		return false;
	}
	return true;
}

static bool read_vt_i2(ThreadSafeOLEStreamReader* reader, S16& i)
{
	U16 string_type;
	if (!reader->readU16(string_type) || string_type != 0x0002)
	{
		docwire_log(error) << "Incorrect value type.";
		return false;
	}
	reader->seek(2, SEEK_CUR); //padding
	reader->readS16(i);
	if (!reader->isValid())
	{
		docwire_log(error) << "OLE Reader error message: " << reader->getLastError();
		return false;
	}
	return true;
}

static bool read_vt_filetime(ThreadSafeOLEStreamReader* reader, tm& time)
{
	U16 type;
	if (!reader->readU16(type) || type != 0x0040)
	{
		docwire_log(error) << "Incorrect variable type.";
		return false;
	}
	reader->seek(2, SEEK_CUR); //padding
	U32 file_time_low, file_time_high;
	reader->readU32(file_time_low);
	reader->readU32(file_time_high);
	if (!reader->isValid())
	{
		docwire_log(error) << "OLE Reader error message: " << reader->getLastError();
		return false;
	}
	if (file_time_low == 0 && file_time_high == 0)
	{
		// Sometimes field exists but date is zero.
		// Last modification time saved by LibreOffice 3.5 when document is created is an example.
		return false;
	}
	unsigned long long int file_time = ((unsigned long long int)file_time_high << 32) | (unsigned long long int)file_time_low;
	time_t t = (time_t)(file_time / 10000000 - 11644473600LL);
  struct tm time_buffer;
  tm* res = thread_safe_gmtime(&t, time_buffer);
	if (res == NULL)
	{
		docwire_log(error) << "Incorrect time value.";
		return false;
	}
	time = *res;
	return true;
}

void parse_oshared_summary_info(ThreadSafeOLEStorage& storage, tag::Metadata& meta)
{
	docwire_log(debug) << "Extracting metadata.";
	if (!storage.isValid())
		throw RuntimeError("Error opening " + storage.name() + " as OLE file");
	ThreadSafeOLEStreamReader* reader = NULL;
	reader = (ThreadSafeOLEStreamReader*)storage.createStreamReader("\005SummaryInformation");
	if (reader == NULL)
		throw RuntimeError("Error opening SummaryInformation stream");
	try
	{
		size_t field_set_stream_start = reader->tell();
		U16 byte_order;
		if (!reader->readU16(byte_order) || byte_order != 0xFFFE)
			throw RuntimeError("Incorrect ByteOrder value");
		U16 version;
		if (!reader->readU16(version) || version != 0x00)
			throw RuntimeError("Incorrect Version value");
		reader->seek(4, SEEK_CUR); //system indentifier
		for (int i = 0; i < 4; i++)
		{
			U32 clsid_part;
			if (!reader->readU32(clsid_part) || clsid_part != 0x00)
				throw RuntimeError("Incorrect CLSID value");
		}
		U32 num_property_sets;
		if (!reader->readU32(num_property_sets) || (num_property_sets != 0x01 && num_property_sets != 0x02))
			throw RuntimeError("Incorrect number of property sets");
		reader->seek(16, SEEK_CUR);	// fmtid0_part
		U32 offset;
		reader->readU32(offset);
		int property_set_pos = field_set_stream_start + offset;
		reader->seek(property_set_pos, SEEK_SET);
		U32 size, num_props = 0;
		reader->readU32(size);
		reader->readU32(num_props);
		for (int i = 0; i < num_props; i++)
		{
			U32 prop_id, offset;
			reader->readU32(prop_id);
			reader->readU32(offset);
			int p = reader->tell();
			switch (prop_id)
			{
				case 0x00000004:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					std::string author;
					if (read_vt_string(reader, author))
					{
						meta.author = author;
					}
					break;
				}
				case 0x00000008:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					std::string last_modified_by;
					if (read_vt_string(reader, last_modified_by))
					{
						meta.last_modified_by = last_modified_by;
					}
					break;
				}
				case 0x0000000C:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					tm creation_date;
					if (read_vt_filetime(reader, creation_date))
					{
						meta.creation_date = creation_date;
					}
					break;
				}
				case 0x0000000D:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					tm last_modification_date;
					if (read_vt_filetime(reader, last_modification_date))
					{
						meta.last_modification_date = last_modification_date;
					}
					break;
				}
				case 0x0000000E:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					S32 page_count;
					if (read_vt_i4(reader, page_count))
					{
						meta.page_count = page_count;
					}
					break;
				}
				case 0x0000000F:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					S32 word_count;
					if (read_vt_i4(reader, word_count))
					{
						meta.word_count = word_count;
					}
					break;
				}
			}
			reader->seek(p, SEEK_SET);
			if (!reader->isValid())
				throw RuntimeError("OLE Reader error message: " + reader->getLastError());
		}
		if (!reader->isValid())
			throw RuntimeError("OLE Reader error message: " + reader->getLastError());
		delete reader;
	}
	catch (std::bad_alloc& ba)
	{
		if (reader)
			delete reader;
		reader = NULL;
		throw;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		throw RuntimeError("Error while parsing SummaryInformation stream", e);
	}
}

static ThreadSafeOLEStreamReader* open_oshared_document_summary_info(ThreadSafeOLEStorage& storage, size_t& field_set_stream_start)
{
	ThreadSafeOLEStreamReader* reader = NULL;
	try
	{
		reader = (ThreadSafeOLEStreamReader*)storage.createStreamReader("\005DocumentSummaryInformation");
		if (reader == NULL)
			throw RuntimeError("Error opening DocumentSummaryInformation stream");
		field_set_stream_start = reader->tell();
		U16 byte_order;
		if (!reader->readU16(byte_order) || byte_order != 0xFFFE)
			throw RuntimeError("Incorrect ByteOrder value");
		U16 version;
		if (!reader->readU16(version) || version != 0x00)
			throw RuntimeError("Incorrect Version value");
		reader->seek(4, SEEK_CUR);	// system indentifier
		for (int i = 0; i < 4; i++)
		{
			U32 clsid_part;;
			if (!reader->readU32(clsid_part) || clsid_part != 0x00)
				throw RuntimeError("Incorrect CLSID value");
		}
		U32 num_property_sets;
		if (!reader->readU32(num_property_sets) || (num_property_sets != 0x01 && num_property_sets != 0x02))
			throw RuntimeError("Incorrect number of property sets");
		reader->seek(16, SEEK_CUR);	//fmtid0_part
		if (!reader->isValid())
			throw RuntimeError("OLE Reader error message: " + reader->getLastError());
		return reader;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		throw RuntimeError("Error while opening DocumentSummaryInformation stream", e);
	}
}

bool get_codepage_from_document_summary_info(ThreadSafeOLEStorage& storage, std::string& codepage)
{
	size_t field_set_stream_start;
	if (!storage.isValid())
		docwire_log(error) << "Error opening " << storage.name() << " as OLE file.";
	ThreadSafeOLEStreamReader* reader = NULL;
	try
	{
		reader = open_oshared_document_summary_info(storage, field_set_stream_start);
		U32 offset;
		reader->readU32(offset);
		int property_set_pos = field_set_stream_start + offset;
		reader->seek(property_set_pos, SEEK_SET);
		U32 size, num_props = 0;
		reader->readU32(size);
		reader->readU32(num_props);
		for (int i = 0; i < num_props; i++)
		{
			U32 prop_id, offset;
			reader->readU32(prop_id);
			reader->readU32(offset);
			int p = reader->tell();
			switch (prop_id)
			{
				case 0x00000001:
					reader->seek(property_set_pos + offset, SEEK_SET);
					S16 icodepage;
					if (!read_vt_i2(reader, icodepage))
					{
						docwire_log(error) << "Error while reading codepage: invalid value.";
						delete reader;
						return false;
					}
					codepage = "cp" + int2string(icodepage);
					delete reader;
					return true;
			}
			reader->seek(p, SEEK_SET);
			if (!reader->isValid())
			{
				docwire_log(error) << "OLE Reader error message: " << reader->getLastError();
				delete reader;
				return false;
			}
		}
		if (!reader->isValid())
		{
			docwire_log(error) << "OLE Reader error message: " << reader->getLastError();
			delete reader;
			return false;
		}
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		docwire_log(error) << "Error while getting codepage info. Error message:" << e.what();
		return false;
	}
	delete reader;
	docwire_log(warning) << "Information about codepage is missing.";
	return false;
}

void parse_oshared_document_summary_info(ThreadSafeOLEStorage& storage, int& slide_count)
{
	docwire_log(debug) << "Extracting additional metadata.";
	size_t field_set_stream_start;
	if (!storage.isValid())
		throw RuntimeError("Error opening " + storage.name() + " as OLE file");
	ThreadSafeOLEStreamReader* reader = NULL;
	try
	{
		reader = open_oshared_document_summary_info(storage, field_set_stream_start);
		U32 offset;
		reader->readU32(offset);
		int property_set_pos = field_set_stream_start + offset;
		reader->seek(property_set_pos, SEEK_SET);
		U32 size, num_props = 0;
		reader->readU32(size);
		reader->readU32(num_props);
		bool slide_count_found = false;
		for (int i = 0; i < num_props; i++)
		{
			U32 prop_id, offset;
			reader->readU32(prop_id);
			reader->readU32(offset);
			int p = reader->tell();
			switch (prop_id)
			{
				case 0x00000007:
					reader->seek(property_set_pos + offset, SEEK_SET);
					if (read_vt_i4(reader, slide_count))
						slide_count_found = true;
					break;
			}
			reader->seek(p, SEEK_SET);
			if (!reader->isValid())
				throw RuntimeError("OLE reader error message: " + reader->getLastError());
		}
		if (!slide_count_found)
			slide_count = -1;
		if (!reader->isValid())
			throw RuntimeError("OLE reader error message: " + reader->getLastError());
		delete reader;
		reader = NULL;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		throw RuntimeError("Error while parsing DocumentSummaryInformation stream", e);
	}
}

} // namespace docwire
