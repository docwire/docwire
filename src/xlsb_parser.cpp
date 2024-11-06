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

#include "xlsb_parser.h"

#include <algorithm>
#include "error_tags.h"
#include "zip_reader.h"
#include <iostream>
#include "log.h"
#include <map>
#include "misc.h"
#include <sstream>
#include <stdint.h>
#include "throw_if.h"
#include <vector>
#include "thread_safe_ole_storage.h"
#include "thread_safe_ole_stream_reader.h"

namespace docwire
{

struct XLSBParser::Implementation
{
	struct XLSBContent
	{
		class ErrorsCodes : public std::map<uint32_t, std::string>
		{
			public:
				ErrorsCodes()
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

		ErrorsCodes m_error_codes;
		std::vector<std::string> m_shared_strings;
		uint32_t m_row_start, m_row_end, m_col_start, m_col_end;
		uint32_t m_current_column, m_current_row;

		XLSBContent()
		{
			m_row_start = 0;
			m_row_end = 0;
			m_col_start = 0;
			m_col_end = 0;
			m_current_column = 0;
			m_current_row = 0;
		}
	};

	const XLSBParser* m_parser;
	XLSBContent m_xlsb_content;

	Implementation(const XLSBParser* parser)
		: m_parser(parser)
	{}

	class XLSBReader
	{
		public:
			enum RecordType
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

			struct Record
			{
				uint32_t m_type;
				uint32_t m_size;

				Record()
				{
					m_type = 0;
					m_size = 0;
				}
			};

		private:
			ZipReader* m_zipfile;
			std::vector<unsigned char> m_chunk;
			int m_chunk_len;
			int m_pointer;
			unsigned long m_file_size;
			unsigned long m_readed;
			std::string m_file_name;

		public:
			XLSBReader(ZipReader& zipfile, const std::string& file_name)
			{
				m_zipfile = &zipfile;
				m_file_name = file_name;
				m_chunk_len = 0;
				m_pointer = 0;
				m_readed = 0;
				m_chunk.reserve(1024);
				m_file_size = 0;
				zipfile.getFileSize(file_name, m_file_size);
			}

			bool done()
			{
				return m_readed == m_file_size;
			}

			void readNum(uint32_t& value, int bytes)
			{
				value = 0;
				throw_if (m_chunk_len - m_pointer < bytes, "Unexpected EOF");
				for (int i = 0; i < bytes; ++i)
					value += (m_chunk[m_pointer++] << (i * 8));
			}

			void readUint8(uint32_t& value)
			{
				readNum(value, 1);
			}

			void readUint16(uint32_t& value)
			{
				readNum(value, 2);
			}

			void readUint32(uint32_t& value)
			{
				readNum(value, 4);
			}

			void readXnum(double& value)
			{
				uint8_t* val_ptr = (uint8_t*)&value;
				throw_if (m_chunk_len - m_pointer < 8, "Unexpected EOF");
				for (int i = 0; i < 8; ++i)
					val_ptr[8 - i] = m_chunk[m_pointer++];
			}

			void readRkNumber(double& value, bool& is_int)
			{
					value = 0;
					uint32_t uvalue;
					readNum(uvalue, 4);
					bool fx100 = (uvalue & 0x00000001) > 0;
					bool fint = (uvalue & 0x00000002) > 0;
					if (fint)
					{
						is_int = true;
						int svalue = (int) uvalue;
						svalue /= 4;	//remove 2 last bits
						if (fx100)
							svalue /= 100;
						value = svalue;
					}
					else
					{
						is_int = false;
						uvalue = uvalue & 0xFFFFFFFC;
						uint64_t* val_ptr = (uint64_t*)&value;
						uint32_t* uval_ptr = (uint32_t*)&uvalue;
						*val_ptr = (uint64_t)*uval_ptr << 32;
						if (fx100)
							value /= 100.0;
					}
			}

			void readXlWideString(std::string& str)
			{
				uint32_t str_size;
					readNum(str_size, 4);
					throw_if (str_size * 2 > m_chunk_len - m_pointer, "XLSB: xlWideString size is larger than chunk size");
					str.reserve(2 * str_size);
					for (int i = 0; i < str_size; ++i)
					{
						unsigned int uc = *((unsigned short*)&m_chunk[m_pointer]);
						m_pointer += 2;
						if (uc != 0)
						{
							if (utf16_unichar_has_4_bytes(uc))
							{
								throw_if (++i >= str_size, "Unexpected EOF");
								uc = (uc << 16) | *((unsigned short*)&m_chunk[m_pointer]);
								m_pointer += 2;
							}
							str += unichar_to_utf8(uc);
						}
					}
			}

			void readRichStr(std::string& str)
			{
					//skip first byte
					throw_if (m_chunk_len == m_pointer, "Unexpected EOF");
					++m_pointer;
					readXlWideString(str);
			}

			void readRecord(Record& record)
			{
				record.m_type = 0;
				record.m_size = 0;
				for (int i = 0; i < 2; ++i)	//read record type
				{
					readChunk(1);
					uint32_t byte = m_chunk[m_pointer++];
					record.m_type += ((byte & 0x7F) << (i * 7));
					if (byte < 128)
						break;
				}
				for (int i = 0; i < 4; ++i)	//read record size
				{
					if (m_pointer == m_chunk_len)
						readChunk(1);
					uint32_t byte = m_chunk[m_pointer++];
					record.m_size += ((byte & 0x7F) << (i * 7));
					if (byte < 128)
						break;
				}
				readChunk(record.m_size);
			}

			void skipBytes(uint32_t bytes_to_skip)
			{
				if (bytes_to_skip <= m_chunk_len - m_pointer)
					m_pointer += bytes_to_skip;
			}

			void readChunk(uint32_t len)
			{
				if (len == 0)
					return;
				m_chunk.resize(len + 1);
				throw_if (!m_zipfile->readChunk(m_file_name, (char*)&m_chunk[0], len, m_chunk_len), "Error reading chunk from zip file", m_file_name);
				m_readed += m_chunk_len;
				throw_if (m_chunk_len != len, "Error reading chunk from zip file", m_file_name);
				m_pointer = 0;
			}
	};

	void parseRecordForSharedStrings(XLSBReader& xlsb_reader, XLSBReader::Record& record)
	{
		switch (record.m_type)
		{
			case XLSBReader::BRT_BEGIN_SST:
			{
				try
				{
					xlsb_reader.skipBytes(4);
					uint32_t strings_count = 0;
					xlsb_reader.readUint32(strings_count);
					m_xlsb_content.m_shared_strings.reserve(strings_count);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_SS_ITEM:
			{
				try
				{
					m_xlsb_content.m_shared_strings.push_back(std::string());
					std::string* new_string = &m_xlsb_content.m_shared_strings[m_xlsb_content.m_shared_strings.size() - 1];
					xlsb_reader.readRichStr(*new_string);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
		}
	}

	void parseColumn(XLSBReader& xlsb_reader, std::string& text)
	{
		uint32_t column;
		xlsb_reader.readUint32(column);
		if (m_xlsb_content.m_current_column > 0)
			text += "	";
		while (column > m_xlsb_content.m_current_column)
		{
			text += "	";
			++m_xlsb_content.m_current_column;
		}
		xlsb_reader.skipBytes(4);
		m_xlsb_content.m_current_column = column + 1;
	}

	void parseRecordForWorksheets(XLSBReader& xlsb_reader, XLSBReader::Record& record, std::string& text)
	{
		switch (record.m_type)
		{
			case XLSBReader::BRT_CELL_BLANK:
			{
				try
				{
					parseColumn(xlsb_reader, text);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_FMLA_ERROR:
			case XLSBReader::BRT_CELL_ERROR:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					uint32_t value;
					xlsb_reader.readUint8(value);
					text += m_xlsb_content.m_error_codes[value];
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_FMLA_BOOL:
			case XLSBReader::BRT_CELL_BOOL:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					uint32_t value;
					xlsb_reader.readUint8(value);
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
			case XLSBReader::BRT_FMLA_NUM:
			case XLSBReader::BRT_CELL_REAL:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					double value;
					xlsb_reader.readXnum(value);
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
			case XLSBReader::BRT_FMLA_STRING:
			case XLSBReader::BRT_CELL_ST:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					xlsb_reader.readXlWideString(text);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_WS_DIM:
			{
				try
				{
					xlsb_reader.readUint32(m_xlsb_content.m_row_start);
					xlsb_reader.readUint32(m_xlsb_content.m_row_end);
					xlsb_reader.readUint32(m_xlsb_content.m_col_start);
					xlsb_reader.readUint32(m_xlsb_content.m_col_end);
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_ROW_HDR:
			{
				try
				{
					uint32_t row;
					xlsb_reader.readUint32(row);
					for (int i = m_xlsb_content.m_current_row; i < row; ++i)
						text += "\n";
					m_xlsb_content.m_current_row = row;
					m_xlsb_content.m_current_column = 0;
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_CELL_RK:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					double value;
					bool is_int;
					xlsb_reader.readRkNumber(value, is_int);
					std::ostringstream os;
					if (!is_int)
						os << value;
					else
						os << (int)value;
					text += os.str();
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
			case XLSBReader::BRT_CELL_ISST:
			{
				try
				{
					parseColumn(xlsb_reader, text);
					uint32_t str_index;
					xlsb_reader.readUint32(str_index);
					if (str_index >= m_xlsb_content.m_shared_strings.size())
						m_parser->sendTag(make_error_ptr("Detected reference to string that does not exist", str_index, m_xlsb_content.m_shared_strings.size()));
					else
						text += m_xlsb_content.m_shared_strings[str_index];
				}
				catch (const std::exception& e)
				{
					std::throw_with_nested(make_error(record.m_type));
				}
				break;
			}
		};
	}

	void parseSharedStrings(ZipReader& unzip)
	{
		XLSBReader::Record record;
		std::string file_name = "xl/sharedStrings.bin";
		if (!unzip.exists(file_name))
		{
			//file may not exist, nothing wrong is with that.
			docwire_log(debug) << "File: " + file_name + " does not exist";
			return;
		}
		XLSBReader xlsb_reader(unzip, file_name);
		while (!xlsb_reader.done())
		{
			try
			{
				xlsb_reader.readRecord(record);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(errors::impl{std::make_pair("file_name", "xl/sharedStrings.bin")});
			}
			try
			{
				parseRecordForSharedStrings(xlsb_reader, record);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(errors::impl{std::make_pair("file_name", "xl/sharedStrings.bin")});
			}
		}
		unzip.closeReadingFileForChunks();
	}

	void parseWorksheets(ZipReader& unzip, std::string& text)
	{
		XLSBReader::Record record;
		int sheet_index = 1;
		std::string sheet_file_name = "xl/worksheets/sheet1.bin";
		while (unzip.exists(sheet_file_name))
		{
			XLSBReader xlsb_reader(unzip, sheet_file_name);
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
					parseRecordForWorksheets(xlsb_reader, record, text);
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

	void parseXLSB(ZipReader& unzip, std::string& text)
	{
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

	void readMetadata(const ZipReader& unzip, attributes::Metadata& metadata)
	{
		docwire_log(debug) << "Extracting metadata.";
		std::string data;
		throw_if (!unzip.read("docProps/app.xml", &data), "Error reading docProps/app.xml");
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
				tm creation_date_tm;
				if (creation_date.length() > 0 && string_to_date(creation_date, creation_date_tm))
					metadata.creation_date = creation_date_tm;
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
				tm last_modification_date_tm;
				if (last_modification_date.length() > 0 && string_to_date(last_modification_date, last_modification_date_tm))
					metadata.last_modification_date = last_modification_date_tm;
			}
		}
	}

};

XLSBParser::XLSBParser()
	: impl(std::make_unique<Implementation>(this))
{
}

XLSBParser::~XLSBParser() = default;

bool XLSBParser::understands(const data_source& data) const
{
	ZipReader unzip{data};
	try
	{
		unzip.open();
	if (!unzip.exists("xl/workbook.bin"))
		return false;
	}
	catch (const std::exception&)
	{
		throw_if (is_encrypted_with_ms_offcrypto(data), errors::file_encrypted{}, "Microsoft Office Document Cryptography");
		return false;
	}
	return true;
}

attributes::Metadata XLSBParser::metaData(const ZipReader& unzip) const
{
	attributes::Metadata metadata;
	try
	{
		impl->readMetadata(unzip, metadata);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error reading metadata"));
	}
	return metadata;
}

void XLSBParser::parse(const data_source& data) const
{
	docwire_log(debug) << "Using XLSB parser.";
	*impl = Implementation{this};
	std::string text;
	ZipReader unzip{data};
	try
	{
		unzip.open();
		sendTag(tag::Document
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
		impl->parseXLSB(unzip, text);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error parsing XLSB"));
	}
	sendTag(tag::Text{.text = text});
	sendTag(tag::CloseDocument{});
}

} // namespace docwire
