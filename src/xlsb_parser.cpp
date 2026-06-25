/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#include "xlsb_parser.h"

#include "charset_converter.h"
#include "attributes.h"
#include "binary_reader.h"
#include "convert_chrono.h" // IWYU pragma: keep
#include <cstring>
#include "data_source.h"
#include "document_elements.h"
#include "error_tags.h"
#include "scoped_stack_push.h"
#include "zip_reader.h"
#include <iostream>
#include "log_entry.h"
#include "log_scope.h"
#include "make_error.h"
#include <map>
#include "misc.h"
#include "serialization_data_source.h" // IWYU pragma: keep
#include <sstream>
#include <stack>
#include <stdint.h>
#include "throw_if.h"
#include <vector>

namespace docwire
{

namespace
{

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.ms-excel.sheet.binary.macroenabled.12"}
};

struct rk_number
{
	double value;
	bool is_int;
};

rk_number read_rk_number(binary::reader& reader)
{
	log_scope();
	uint32_t uvalue = reader.read_little_endian<uint32_t>();
	bool fx100 = (uvalue & 0x00000001) > 0;
	bool fint = (uvalue & 0x00000002) > 0;
	if (fint)
	{
		int svalue = (int) uvalue;
		svalue /= 4;	//remove 2 last bits
		double final_value = svalue;
		if (fx100)
			final_value /= 100.0;
		return { final_value, true };
	}
	else
	{
		uvalue = uvalue & 0xFFFFFFFC;
		uint64_t temp_val = (uint64_t)uvalue << 32;
		double final_value = std::bit_cast<double>(temp_val);
		if (fx100)
			final_value /= 100.0;
		return { final_value, false };
	}
}

std::string read_xl_wide_string(binary::reader& reader)
{
	log_scope();
	const uint32_t num_chars = reader.read_little_endian<uint32_t>();
	log_entry(num_chars);
	if (num_chars == 0)
		return "";

	// The string is stored as a raw sequence of UTF-16LE bytes.
	// We read it as a raw buffer and pass it directly to the charset_converter,
	// which is configured to expect "UTF-16LE".
	const size_t buffer_byte_size = num_chars * sizeof(char16_t);
	std::vector<char> utf16_bytes(buffer_byte_size);
	reader.read({reinterpret_cast<std::byte*>(utf16_bytes.data()), utf16_bytes.size()});

	thread_local charset_converter conv("UTF-16LE", "UTF-8");
	return conv.convert({utf16_bytes.data(), utf16_bytes.size()});
}

std::string read_rich_str(binary::reader& reader)
{
	log_scope();
	reader.read_little_endian<uint8_t>(); // skip flags
	return read_xl_wide_string(reader);
}

} // anonymous namespace

template<>
struct pimpl_impl<xlsb_parser> : pimpl_impl_base
{
	void parse(const data_source& data, const message_callbacks& emit_message);
	attributes::metadata metaData(zip_reader& unzip);
	struct xlsb_content
	{
		class errors_codes : public std::map<uint32_t, std::string>
		{
			public:
				errors_codes()
				{
					insert(std::pair<uint32_t, std::string>(0x00, "#NULL!"));
					insert(std::pair<uint32_t, std::string>(0x07, "#DIV/0!"));
					insert(std::pair<uint32_t, std::string>(0x0F, "#VALUE!"));
					insert(std::pair<uint32_t, std::string>(0x17, "#REF!"));
					insert(std::pair<uint32_t, std::string>(0x1D, "#NAME?"));
					insert(std::pair<uint32_t, std::string>(0x24, "#NUM!"));
					insert(std::pair<uint32_t, std::string>(0x2A, "#N/A"));
					insert(std::pair<uint32_t, std::string>(0x2B, "#GETTING_DATA"));
				}
		};

		errors_codes m_error_codes;
		std::vector<std::string> m_shared_strings;
		uint32_t m_row_start, m_row_end, m_col_start, m_col_end;
		uint32_t m_current_column, m_current_row;

		xlsb_content()
		{
			m_row_start = 0;
			m_row_end = 0;
			m_col_start = 0;
			m_col_end = 0;
			m_current_column = 0;
			m_current_row = 0;
		}
	};

	struct context
	{
		const message_callbacks& emit_message;
		xlsb_content content;
	};
	std::stack<context> m_context_stack;

	template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

	xlsb_content& content()
	{
		return m_context_stack.top().content;
	}

	class xlsb_reader
	{
		public:
			enum record_type
			{
				BRT_ROW_HDR = 0x0,
				BRT_CELL_BLANK = 0x1,
				BRT_CELL_RK = 0x2,
				BRT_CELL_ERROR = 0x3,
				BRT_CELL_BOOL = 0x4,
				BRT_CELL_REAL = 0x5,
				BRT_CELL_ST = 0x6,
				BRT_CELL_ISST = 0x7,
				BRT_FMLA_STRING = 0x8,
				BRT_FMLA_NUM = 0x9,
				BRT_FMLA_BOOL = 0xA,
				BRT_FMLA_ERROR = 0xB,
				BRT_SS_ITEM = 0x13,
				BRT_WS_DIM = 0x94,
				BRT_BEGIN_SST = 0x9F
			};

			struct record
			{
				uint32_t m_type;
				uint32_t m_size;

				record()
				{
					m_type = 0;
					m_size = 0;
				}
			};

		private:
			zip_reader* m_zipfile;
			unsigned long m_file_size;
			unsigned long m_read_total;
			std::string m_file_name;
			binary::reader m_stream_reader;

		public:
			xlsb_reader(zip_reader& zipfile, const std::string& file_name)
				: m_zipfile(&zipfile),
					m_file_name(file_name),
					m_read_total(0),
					m_file_size(0),
					m_stream_reader([this](std::span<std::byte> dest)
					{
						int read_now = 0;
						// We are reading raw binary data, so we do not want readChunk to add a null terminator.
						if (!m_zipfile->readChunk(m_file_name, (char*)dest.data(), dest.size(), read_now, false))
							throw make_error("Failed to read chunk from zip", m_file_name, errors::uninterpretable_data{});

						throw_if(static_cast<size_t>(read_now) != dest.size(), "Unexpected EOF reading from zip chunk", m_file_name, errors::uninterpretable_data{});

						m_read_total += read_now;
						if (m_read_total == m_file_size)
							m_zipfile->closeReadingFileForChunks();
					})
			{
				m_zipfile->getFileSize(file_name, m_file_size);
			}

			bool done()
			{
				return m_read_total == m_file_size;
			}

			void readRecord(record& record)
			{
				log_scope();
				record.m_type = 0;
				record.m_size = 0;
				for (int i = 0; i < 2; ++i)	//read record type
				{
					uint8_t byte = m_stream_reader.read_little_endian<uint8_t>();
					record.m_type += ((byte & 0x7F) << (i * 7));
					if (byte < 128)
						break;
				}
				for (int i = 0; i < 4; ++i)	//read record size
				{
					uint8_t byte = m_stream_reader.read_little_endian<uint8_t>();
					record.m_size += ((byte & 0x7F) << (i * 7));
					if (byte < 128)
						break;
				}
			}

			// This function reads the record payload from the main stream into a temporary buffer
			// and returns a new reader that operates only on that buffer.
			binary::reader record_reader(uint32_t record_size)
			{
				log_scope();
				std::vector<std::byte> record_bytes(record_size);
				m_stream_reader.read(record_bytes);
				return binary::reader([bytes = std::move(record_bytes), pos = 0](std::span<std::byte> dest) mutable {
					throw_if(pos + dest.size() > bytes.size(), "Reading past record boundary", errors::uninterpretable_data{});
					std::memcpy(dest.data(), bytes.data() + pos, dest.size());
					pos += dest.size();
				});
			}
	};

	void parseRecordForSharedStrings(binary::reader& record_reader, xlsb_reader::record& record)
	{
		log_scope(record.m_type);
		switch (record.m_type)
		{
			case xlsb_reader::BRT_BEGIN_SST:
			{
				try
				{
					// This record contains the total number of strings and the count of unique strings.
					// We only need the total count to reserve space.
					uint32_t total_strings;
					total_strings = record_reader.read_little_endian<uint32_t>();
					content().m_shared_strings.reserve(total_strings);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_SS_ITEM:
			{
				try
				{
					content().m_shared_strings.push_back(std::string());
					std::string* new_string = &content().m_shared_strings[content().m_shared_strings.size() - 1];
					*new_string = read_rich_str(record_reader);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
		}
	}

	void parseColumn(binary::reader& record_reader, std::string& text)
	{
		log_scope();
		uint32_t column = record_reader.read_little_endian<uint32_t>();
		if (content().m_current_column > 0)
			text += "	";
		while (column > content().m_current_column)
		{
			text += "	";
			++content().m_current_column;
		}
		record_reader.read_little_endian<uint32_t>(); // Skip xf_index
		content().m_current_column = column + 1;
	}

	void parseRecordForWorksheets(binary::reader& record_reader, xlsb_reader::record& record, std::string& text)
	{
		log_scope(record.m_type);
		switch (record.m_type)
		{
			case xlsb_reader::BRT_CELL_BLANK:
			{
				try
				{
					parseColumn(record_reader, text);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_FMLA_ERROR:
			case xlsb_reader::BRT_CELL_ERROR:
			{
				try
				{
					parseColumn(record_reader, text);
					uint8_t value = record_reader.read_little_endian<uint8_t>();
					text += content().m_error_codes[value];
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_FMLA_BOOL:
			case xlsb_reader::BRT_CELL_BOOL:
			{
				try
				{
					parseColumn(record_reader, text);
					uint8_t value = record_reader.read_little_endian<uint8_t>();
					if (value)
						text += "1";
					else
						text += "0";
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_FMLA_NUM:
			case xlsb_reader::BRT_CELL_REAL:
			{
				try
				{
					parseColumn(record_reader, text);
					double value;
					value = record_reader.read_double_le();
					std::ostringstream os;
					os << value;
					text += os.str();
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_FMLA_STRING:
			case xlsb_reader::BRT_CELL_ST:
			{
				try
				{
					parseColumn(record_reader, text);
					text += read_xl_wide_string(record_reader);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_WS_DIM:
			{
				try
				{
					content().m_row_start = record_reader.read_little_endian<uint32_t>();
					content().m_row_end = record_reader.read_little_endian<uint32_t>();
					content().m_col_start = record_reader.read_little_endian<uint32_t>();
					content().m_col_end = record_reader.read_little_endian<uint32_t>();
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_ROW_HDR:
			{
				try
				{
					log_scope();
					uint32_t row = record_reader.read_little_endian<uint32_t>();
					log_entry(content().m_current_row, row);
					for (int i = content().m_current_row; i < row; ++i)
						text += "\n";
					content().m_current_row = row;
					content().m_current_column = 0;
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_CELL_RK:
			{
				try
				{
					parseColumn(record_reader, text);
					rk_number rk = read_rk_number(record_reader);
					if (rk.is_int)
						text += stringify((int)rk.value);
					else
						text += stringify(rk.value);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case xlsb_reader::BRT_CELL_ISST:
			{
				try
				{
					parseColumn(record_reader, text);
					uint32_t str_index = record_reader.read_little_endian<uint32_t>();
					if (str_index >= content().m_shared_strings.size())
						emit_message(make_error_ptr("Detected reference to string that does not exist", str_index, content().m_shared_strings.size()));
					else
						text += content().m_shared_strings[str_index];
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
		};
	}

	void parseSharedStrings(zip_reader& unzip)
	{
		log_scope();
		xlsb_reader::record record;
		std::string file_name = "xl/sharedStrings.bin";
		if (!unzip.exists(file_name))
		{
			//file may not exist, nothing wrong is with that.
			log_entry();
			return;
		}
		xlsb_reader xlsb_reader(unzip, file_name);
		while (!xlsb_reader.done())
		{
			try
			{
				xlsb_reader.readRecord(record);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(make_error(std::make_pair("file_name", "xl/sharedStrings.bin")));
			}
			try
			{
				auto record_reader = xlsb_reader.record_reader(record.m_size);
				parseRecordForSharedStrings(record_reader, record);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(make_error(std::make_pair("file_name", "xl/sharedStrings.bin")));
			}
		}
		unzip.closeReadingFileForChunks();
	}

	void parseWorksheets(zip_reader& unzip, std::string& text)
	{
		log_scope();
		xlsb_reader::record record;
		int sheet_index = 1;
		std::string sheet_file_name = "xl/worksheets/sheet1.bin";
		while (unzip.exists(sheet_file_name))
		{
			xlsb_reader xlsb_reader(unzip, sheet_file_name);
			while (!xlsb_reader.done())
			{
				try
				{
					xlsb_reader.readRecord(record);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(sheet_file_name));
				}
				try
				{
					auto record_reader = xlsb_reader.record_reader(record.m_size);
					parseRecordForWorksheets(record_reader, record, text);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(sheet_file_name));
				}
			}
			unzip.closeReadingFileForChunks();
			++sheet_index;
			std::ostringstream os;
			os << sheet_index;
			sheet_file_name = "xl/worksheets/sheet" + os.str() + ".bin";
			text += "\n\n";
		}
	}

	void parseXLSB(zip_reader& unzip, std::string& text)
	{
		log_scope();
		text.reserve(1024 * 1024);
		throw_if (!unzip.loadDirectory(), "Error loading zip directory");
		try
		{
			parseSharedStrings(unzip);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error("parseSharedStrings() failed"));
		}
		try
		{
			parseWorksheets(unzip, text);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(make_error("parseWorksheets() failed"));
		}
	}

	void readMetadata(zip_reader& unzip, attributes::metadata& metadata)
	{
		log_scope();
		std::string data;
		throw_if (!unzip.read("docProps/app.xml", &data), "Error reading docProps/app.xml", errors::uninterpretable_data{});
		if (data.find("<TitlesOfParts>") != std::string::npos && data.find("</TitlesOfParts>") != std::string::npos)
		{
			data.erase(data.find("</TitlesOfParts>"));
			data.erase(0, data.find("<TitlesOfParts>") + 15);
			size_t pages_count_start = data.find("<vt:vector size=\"");
			if (pages_count_start != std::string::npos)
				metadata.page_count = strtol(data.c_str() + pages_count_start + 17, NULL, 10);
		}
		data.clear();
		throw_if (!unzip.read("docProps/core.xml", &data), "Error reading docProps/core.xml");
		bool author_exist = data.find("<dc:creator/>") == std::string::npos && data.find("<dc:creator") != std::string::npos;
		if (author_exist)
		{
			size_t author_start = data.find("<dc:creator");
			size_t author_end = data.find("</dc:creator>");
			std::string author = data.substr(author_start + 11, author_end - author_start - 11);
			if (author.find(">") != std::string::npos)
			{
				author.erase(0, author.find(">") + 1);
				if (author.length() > 0)
					metadata.author = author;
			}
		}
		bool last_modify_by_exist = data.find("<cp:lastModifiedBy/>") == std::string::npos && data.find("<cp:lastModifiedBy") != std::string::npos;
		if (last_modify_by_exist)
		{
			size_t last_modify_by_start = data.find("<cp:lastModifiedBy");
			size_t last_modify_by_end = data.find("</cp:lastModifiedBy>");
			std::string last_modify_by = data.substr(last_modify_by_start + 18, last_modify_by_end - last_modify_by_start - 18);
			if (last_modify_by.find(">") != std::string::npos)
			{
				last_modify_by.erase(0, last_modify_by.find(">") + 1);
				if (last_modify_by.length() > 0)
					metadata.last_modified_by = last_modify_by;
			}
		}
		bool creation_date_exist = data.find("<dcterms:created/>") == std::string::npos && data.find("<dcterms:created") != std::string::npos;
		if (creation_date_exist)
		{
			size_t creation_date_start = data.find("<dcterms:created");
			size_t creation_date_end = data.find("</dcterms:created>");
			std::string creation_date = data.substr(creation_date_start + 16, creation_date_end - creation_date_start - 16);
			if (creation_date.find(">") != std::string::npos)
			{
				creation_date.erase(0, creation_date.find(">") + 1);
				if (auto creation_date_tp = convert::try_to<std::chrono::sys_seconds>(with::date_format::iso8601{creation_date}))
					metadata.creation_date = creation_date_tp;
			}
		}
		bool last_modification_date_exist = data.find("<dcterms:modified/>") == std::string::npos && data.find("<dcterms:modified") != std::string::npos;
		if (last_modification_date_exist)
		{
			size_t last_modification_date_start = data.find("<dcterms:modified");
			size_t last_modification_date_end = data.find("</dcterms:modified>");
			std::string last_modification_date = data.substr(last_modification_date_start + 17, last_modification_date_end - last_modification_date_start - 17);
			if (last_modification_date.find(">") != std::string::npos)
			{
				last_modification_date.erase(0, last_modification_date.find(">") + 1);
				if (auto last_modification_date_tp = convert::try_to<std::chrono::sys_seconds>(with::date_format::iso8601{last_modification_date}))
					metadata.last_modification_date = last_modification_date_tp;
			}
		}
	}

};

xlsb_parser::xlsb_parser() = default;

attributes::metadata pimpl_impl<xlsb_parser>::metaData(zip_reader& unzip)
{
	attributes::metadata metadata;
	try
	{
		readMetadata(unzip, metadata);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error reading metadata"));
	}
	return metadata;
}

void pimpl_impl<xlsb_parser>::parse(const data_source& data, const message_callbacks& emit_message)
{
	log_scope(data);
	scoped::stack_push<pimpl_impl<xlsb_parser>::context> context_guard{m_context_stack, {emit_message}};
	std::string text;
	zip_reader unzip{data};
	try
	{
		unzip.open();
		emit_message(document::document
			{
				.metadata = [this, &unzip]() { return metaData(unzip); }
			});
	}
	catch (const std::exception&)
	{
		if (is_encrypted_with_ms_offcrypto(data))
			std::throw_with_nested(make_error(errors::file_encrypted{}, "Microsoft Office Document Cryptography"));
		std::throw_with_nested(make_error("Failed to open zip archive"));
	}
	try
	{
		parseXLSB(unzip, text);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error parsing XLSB"));
	}
	emit_message(document::text{.text = text});
	emit_message(document::close_document{});
}

continuation xlsb_parser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	impl().parse(data, emit_message);
	return continuation::proceed;
}

} // namespace docwire
