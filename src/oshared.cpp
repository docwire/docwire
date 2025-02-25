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

#include "error_tags.h"
#include "misc.h"
#include "log.h"
#include "throw_if.h"
#include "wv2/utilities.h"
#include <time.h>
#include "thread_safe_ole_stream_reader.h"
#include "thread_safe_ole_storage.h"

namespace docwire
{

using namespace wvWare;

static std::string read_vt_string(ThreadSafeOLEStreamReader* reader)
{
	U16 string_type;
	throw_if (!reader->readU16(string_type), reader->getLastError());
	throw_if (string_type != 0x1E, "Incorrect string type.", errors::uninterpretable_data{});
	reader->seek(2, SEEK_CUR); //padding
	U32 len;
	reader->readU32(len);
	std::string s;
	for (int j = 0; j < len - 1; j++)
	{
		S8 ch;
		reader->readS8(ch);
		if (ch == '\0')
			break;
		s += ch;
	}
	throw_if (!reader->isValid(), "OLE Reader error", reader->getLastError());
	return s;
}

static S32 read_vt_i4(ThreadSafeOLEStreamReader* reader)
{
	U16 string_type;
	throw_if (!reader->readU16(string_type), reader->getLastError());
	throw_if (string_type != 0x0003, "Incorrect value type.", errors::uninterpretable_data{});
	reader->seek(2, SEEK_CUR); //padding
	S32 i;
	reader->readS32(i);
	throw_if (!reader->isValid(), reader->getLastError());
	return i;
}

static S16 read_vt_i2(ThreadSafeOLEStreamReader* reader)
{
	U16 string_type;
	throw_if (!reader->readU16(string_type));
	throw_if (string_type != 0x0002, "Incorrect value type.", errors::uninterpretable_data{});
	reader->seek(2, SEEK_CUR); //padding
	S16 i;
	reader->readS16(i);
	throw_if (!reader->isValid(), "OLE Reader error", reader->getLastError());
	return i;
}

static tm read_vt_filetime(ThreadSafeOLEStreamReader* reader)
{
	U16 type;
	throw_if (!reader->readU16(type), reader->getLastError());
	throw_if (type != 0x0040, "Incorrect variable type.", errors::uninterpretable_data{});
	reader->seek(2, SEEK_CUR); //padding
	U32 file_time_low, file_time_high;
	reader->readU32(file_time_low);
	reader->readU32(file_time_high);
	throw_if (!reader->isValid(), "OLE Reader error", reader->getLastError());
	// Sometimes field exists but date is zero.
	// Last modification time saved by LibreOffice 3.5 when document is created is an example.
	throw_if (file_time_low == 0 && file_time_high == 0,
		"Filetime field value is zero.", errors::uninterpretable_data{});
	unsigned long long int file_time = ((unsigned long long int)file_time_high << 32) | (unsigned long long int)file_time_low;
	// Sometimes field exists, date is zero (1601-01-01) but time is not.
	// Last modification time saved by LibreOffice 3.5 when document is created is an example.
	throw_if (file_time < 864000000000LL, "Incorrect filetime value (1601-01-01).", errors::uninterpretable_data{});
	docwire_log_vars(file_time, file_time_low, file_time_high);
	time_t t = (time_t)(file_time / 10000000 - 11644473600LL);
  struct tm time_buffer;
  tm* res = thread_safe_gmtime(&t, time_buffer);
	throw_if (res == NULL, "Incorrect time value.", errors::uninterpretable_data{});
	return *res;
}

void parse_oshared_summary_info(ThreadSafeOLEStorage& storage, attributes::Metadata& meta, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
	docwire_log(debug) << "Extracting metadata.";
	throw_if (!storage.isValid(), storage.getLastError(), storage.name());
	ThreadSafeOLEStreamReader* reader = NULL;
	reader = (ThreadSafeOLEStreamReader*)storage.createStreamReader("\005SummaryInformation");
	throw_if (reader == NULL, storage.getLastError());
	try
	{
		size_t field_set_stream_start = reader->tell();
		U16 byte_order;
		throw_if (!reader->readU16(byte_order), reader->getLastError());
		throw_if (byte_order != 0xFFFE, "Unexpected byte order.", errors::uninterpretable_data{});
		U16 version;
		throw_if (!reader->readU16(version), reader->getLastError());
		throw_if (version != 0x0000, "Unexpected version.", errors::uninterpretable_data{});
		reader->seek(4, SEEK_CUR); //system indentifier
		for (int i = 0; i < 4; i++)
		{
			U32 clsid_part;
			throw_if (!reader->readU32(clsid_part), reader->getLastError());
			throw_if (clsid_part != 0x00, "Unexpected clsid part.", errors::uninterpretable_data{});
		}
		U32 num_property_sets;
		throw_if (!reader->readU32(num_property_sets), "Error reading number of property sets");
		throw_if (num_property_sets != 0x01 && num_property_sets != 0x02,
			"Unexpected number of property sets", num_property_sets, errors::uninterpretable_data{});
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
					try
					{
						meta.author = read_vt_string(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading author.")));
					}
					break;
				}
				case 0x00000008:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					try
					{
						meta.last_modified_by = read_vt_string(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading last modified by.")));
					}
					break;
				}
				case 0x0000000C:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					try
					{
						meta.creation_date = read_vt_filetime(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading creation date.")));
					}
					break;
				}
				case 0x0000000D:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					try
					{
						meta.last_modification_date = read_vt_filetime(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading last modification date.")));
					}
					break;
				}
				case 0x0000000E:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					try
					{
						meta.page_count = read_vt_i4(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading page count.")));
					}
					break;
				}
				case 0x0000000F:
				{
					reader->seek(property_set_pos + offset, SEEK_SET);
					try
					{
						meta.word_count = read_vt_i4(reader);
					}
					catch (const std::exception&)
					{
						non_fatal_error_handler(errors::make_nested_ptr(std::current_exception(), make_error("Error reading word count.")));
					}
					break;
				}
			}
			reader->seek(p, SEEK_SET);
			throw_if (!reader->isValid(), "Error reading property set", p, reader->getLastError());
		}
		throw_if (!reader->isValid());
		delete reader;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		std::throw_with_nested(make_error("Error parsing summary information"));
	}
}

static ThreadSafeOLEStreamReader* open_oshared_document_summary_info(ThreadSafeOLEStorage& storage, size_t& field_set_stream_start)
{
	ThreadSafeOLEStreamReader* reader = NULL;
	try
	{
		reader = (ThreadSafeOLEStreamReader*)storage.createStreamReader("\005DocumentSummaryInformation");
		throw_if (reader == NULL, storage.getLastError(), std::make_pair("stream_path", "\005DocumentSummaryInformation"));
		field_set_stream_start = reader->tell();
		U16 byte_order;
		throw_if (!reader->readU16(byte_order), reader->getLastError());
		throw_if (byte_order != 0xFFFE, "Incorrect byte order.", errors::uninterpretable_data{});
		U16 version;
		throw_if (!reader->readU16(version), reader->getLastError());
		throw_if (version != 0x0000, "Unsupported version.", errors::uninterpretable_data{});
		reader->seek(4, SEEK_CUR);	// system indentifier
		for (int i = 0; i < 4; i++)
		{
			U32 clsid_part;;
			throw_if (!reader->readU32(clsid_part), reader->getLastError());
			throw_if (clsid_part != 0x00, "Unsupported clsid.", errors::uninterpretable_data{});
		}
		U32 num_property_sets;
		throw_if (!reader->readU32(num_property_sets), reader->getLastError());
		throw_if (num_property_sets != 0x01 && num_property_sets != 0x02, "Unsupported number of property sets.", errors::uninterpretable_data{});
		reader->seek(16, SEEK_CUR);	//fmtid0_part
		throw_if (!reader->isValid(), "Error skipping fmtid0", reader->getLastError());
		return reader;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		throw;
	}
}

std::string get_codepage_from_document_summary_info(ThreadSafeOLEStorage& storage)
{
	size_t field_set_stream_start;
	throw_if (!storage.isValid(), "Error opening storage as OLE file.", storage.name());
	try
	{
		std::unique_ptr<ThreadSafeOLEStreamReader> reader { open_oshared_document_summary_info(storage, field_set_stream_start) };
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
					try
					{
						icodepage = read_vt_i2(reader.get());
					}
					catch (const std::exception& e)
					{
						std::throw_with_nested(make_error("Error while reading codepage: invalid value."));
					}
					return "cp" + int2string(icodepage);
			}
			reader->seek(p, SEEK_SET);
			throw_if (!reader->isValid(), "OLE Reader error", reader->getLastError());
		}
		throw_if (!reader->isValid(), "OLE Reader error", reader->getLastError());
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error while getting codepage info."));
	}
	throw make_error("Information about codepage is missing.");
}

void parse_oshared_document_summary_info(ThreadSafeOLEStorage& storage, int& slide_count)
{
	docwire_log(debug) << "Extracting additional metadata.";
	size_t field_set_stream_start;
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
					try
					{
						slide_count = read_vt_i4(reader);
						slide_count_found = true;
					}
					catch (const std::exception& e)
					{}
					break;
			}
			reader->seek(p, SEEK_SET);
			throw_if (!reader->isValid(), "Error seeking to property", p, reader->getLastError());
		}
		if (!slide_count_found)
			slide_count = -1;
		throw_if (!reader->isValid());
		delete reader;
		reader = NULL;
	}
	catch (const std::exception& e)
	{
		if (reader)
			delete reader;
		reader = NULL;
		std::throw_with_nested(make_error("Error while parsing document summary info"));
	}
}

} // namespace docwire
