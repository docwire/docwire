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

#include "xls_parser.h"

#include "data_source.h"
#include "document_elements.h"
#include "error_tags.h"
#include "log.h"
#include <map>
#include <math.h>
#include "misc.h"
#include <mutex>
#include "oshared.h"
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scoped_stack_push.h"
#include "throw_if.h"
#include "wv2/src/textconverter.h"
#include "wv2/src/utilities.h"
#include <vector>
#include <time.h>
#include "thread_safe_ole_stream_reader.h"
#include "thread_safe_ole_storage.h"

using namespace wvWare;

namespace docwire
{

namespace
{
	std::mutex xls_converter_mutex;
	std::mutex parser_mutex;

enum RecordType
{
	XLS_BOF = 0x809,
	XLS_CODEPAGE = 0x42,
	XLS_BLANK = 0x201,
	XLS_CONTINUE = 0x3C,
	XLS_DATE_1904 = 0x22,
	XLS_FILEPASS = 0x2F,
	XLS_FORMAT = 0x41E,
	XLS_FORMULA = 0x06,
	XLS_INTEGER_CELL = 0x202, // Not in MS specification
	XLS_LABEL = 0x204,
	XLS_RSTRING = 0xD6,
	XLS_LABEL_SST = 0xFD,
	XLS_MULBLANK = 0xBE,
	XLS_MULRK = 0xBD,
	XLS_NUMBER = 0x203,
	XLS_RK = 0x27E,
	XLS_SST = 0xFC,
	XLS_STRING = 0x207,
	XLS_XF = 0xE0,
	XLS_EOF = 0x0A
};

enum BiffVersion { BIFF2, BIFF3, BIFF4, BIFF5, BIFF8 };

struct XFRecord
{
	short int num_format_id;
};

struct context
{
	const message_callbacks& emit_message;
	std::string m_codepage = "cp1251";
	BiffVersion m_biff_version;
	std::vector<XFRecord> m_xf_records;
	double m_date_shift;
	std::vector<std::string> m_shared_string_table;
	std::vector<unsigned char> m_shared_string_table_buf;
	std::vector<size_t> m_shared_string_table_record_sizes;
	int m_prev_rec_type;
	int m_last_string_formula_row;
	int m_last_string_formula_col;
	std::set<int> m_defined_num_format_ids;
	int m_last_row, m_last_col;
};

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.ms-excel"},
	mime_type{"application/vnd.ms-excel.sheet.macroenabled.12"},
	mime_type{"application/vnd.ms-excel.template.macroenabled.12"}
};

} // anonymous namespace

template<>
struct pimpl_impl<XLSParser> : pimpl_impl_base
{
	std::stack<context> m_context_stack;
	
	template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

	void parse(const data_source& data, const message_callbacks& emit_message);
	std::string parse(ThreadSafeOLEStorage& storage, const message_callbacks& emit_message);

	U16 getU16LittleEndian(std::vector<unsigned char>::const_iterator buffer)
	{
		return (unsigned short int)(*buffer) | ((unsigned short int)(*(buffer + 1)) << 8);
	}
	
	S32 getS32LittleEndian(std::vector<unsigned char>::const_iterator buffer)
	{
		return (long)(*buffer) | ((long)(*(buffer + 1)) << 8L) | ((long)(*(buffer + 2)) << 16L)|((long)(*(buffer + 3)) << 24L);
	}  

	class StandardDateFormats : public std::map<int, std::string>
	{
		public:
			StandardDateFormats()
			{
				insert(value_type(0x0E, "%m-%d-%y"));
				insert(value_type(0x0F, "%d-%b-%y"));
				insert(value_type(0x10, "%d-%b"));
				insert(value_type(0x11, "%b-%d"));
				insert(value_type(0x12, "%l:%M %p"));
				insert(value_type(0x13, "%l:%M:%S %p"));
				insert(value_type(0x14, "%H:%M"));
				insert(value_type(0x15, "%H:%M:%S"));
				insert(value_type(0x16, "%m-%d-%y %H:%M"));
				insert(value_type(0x2d, "%M:%S"));
				insert(value_type(0x2e, "%H:%M:%S"));
				insert(value_type(0x2f, "%M:%S"));
				insert(value_type(0xa4, "%m.%d.%Y %l:%M:%S %p"));
			}
	};

	bool oleEof(ThreadSafeOLEStreamReader& reader)
	{
		return reader.tell() == reader.size();
	}

	std::string getStandardDateFormat(int xf_index)
	{
		static StandardDateFormats formats;
		if (xf_index >= m_context_stack.top().m_xf_records.size())
		{
			emit_message(make_error_ptr("Incorrect format code", xf_index));
			return "";
		}
		int num_format_id = m_context_stack.top().m_xf_records[xf_index].num_format_id;
		if (m_context_stack.top().m_defined_num_format_ids.count(num_format_id))
			return "";
		StandardDateFormats::iterator i = formats.find(num_format_id);
		if (i == formats.end())
			return "";
		else
			return i->second;
	}	

	std::string xlsDateToString(double xls_date, std::string date_fmt)
	{
		time_t time = rint((xls_date - m_context_stack.top().m_date_shift) * 86400);
		char buffer[128];
    struct tm time_buffer;
		strftime(buffer, 127, date_fmt.c_str(), thread_safe_gmtime(&time, time_buffer));
		return buffer;
	}

	std::string formatXLSNumber(double number, short int xf_index)
	{
		std::string date_fmt = getStandardDateFormat(xf_index);
		if (date_fmt != "")
			return xlsDateToString(number, date_fmt);
		else
		{
			char buffer[128];
			sprintf(buffer,"%.12g",number);
			return buffer;
		}
	}

	std::string parseXNum(std::vector<unsigned char>::const_iterator src,int xf_index)
	{
		union
		{
			unsigned char xls_num[8];
			double num;
		} xnum_conv;
		#ifdef WORDS_BIGENDIAN
			std::reverse_copy(src, src + 8, xnum_conv.xls_num)
		#else
			std::copy(src, src + 8, xnum_conv.xls_num);
		#endif
		return formatXLSNumber(xnum_conv.num, xf_index);
	}

	std::string parseRkRec(std::vector<unsigned char>::const_iterator src, short int xf_index)
	{
		double number;
		if ((*src) & 0x02)
			number = (double)(getS32LittleEndian(src) >> 2);
		else
		{
			union
			{
				unsigned char xls_num[8];
				double num;
			} rk_num_conv;
			std::fill(rk_num_conv.xls_num, rk_num_conv.xls_num + 8, '\0');
		#ifdef WORDS_BIGENDIAN
			std:reverse_copy(src, src + 4, dconv.xls_num);
			rk_num_conv.xls_num[0] &= 0xfc;
		#else
			std::copy(src, src + 4, rk_num_conv.xls_num + 4);
			rk_num_conv.xls_num[3] &= 0xfc;
		#endif
			number = rk_num_conv.num;
		}
		if ((*src) & 0x01)
			number *= 0.01;
		return formatXLSNumber(number, xf_index);
	}

	std::string parseXLUnicodeString(std::vector<unsigned char>::const_iterator* src, std::vector<unsigned char>::const_iterator src_end, const std::vector<size_t>& record_sizes, size_t& record_index, size_t& record_pos)
	{
		if (record_pos >= record_sizes[record_index])
		{
			size_t diff = record_pos - record_sizes[record_index];
			if (diff > 0)
				emit_message(make_error_ptr("XLUnicodeString starts after record boundary", diff));
			record_pos = diff;
			record_index++;
		}
		/*
			This part is mostly based on OpenOffice/LibreOffice XLS format documentation and filter source code.
			record id   BIFF    ->  XF type     String type
			0x0004      2-7     ->  3 byte      8-bit length, byte string
			0x0004      8       ->  3 byte      16-bit length, unicode string
			0x0204      2-7     ->  2 byte      16-bit length, byte string
			0x0204      8       ->  2 byte      16-bit length, unicode string
		*/
		// warning TODO: Add support for record 0x0004 (in BIFF2).
		if (src_end - *src < 2)
		{
			emit_message(make_error_ptr("Unexpected end of buffer."));
			*src = src_end;
			return "";
		}
		int count = getU16LittleEndian(*src);
		*src += 2;
		record_pos += 2;
		if (src_end - *src < 1)
		{
			emit_message(make_error_ptr("Unexpected end of buffer."));
			*src = src_end;
			return "";
		}
		int flags = 0;
		if (m_context_stack.top().m_biff_version >= BIFF8)
		{
			flags = **src;
			*src += 1;
			record_pos += 1;
		}
		int char_size = (flags & 0x01) ? 2 : 1;
		int after_text_block_len = 0;
		if (flags & 0x08) // rich text
		{
			docwire_log(debug) << "Rich text flag enabled.";
			if (src_end - *src < 2)
			{
				emit_message(make_error_ptr("Unexpected end of buffer."));
				*src = src_end;
				return "";
			}
			after_text_block_len += 4*getU16LittleEndian(*src);
			*src += 2;
			record_pos += 2;
		}
		if (flags & 0x04) // asian
		{
			docwire_log(debug) << "Asian flag enabled.";
			if (src_end - *src < 4)
			{
				emit_message(make_error_ptr("Unexpected end of buffer."));
				*src = src_end;
				return "";
			}
			after_text_block_len += getS32LittleEndian(*src);
			*src += 4;
			record_pos += 4;
		}
		if (after_text_block_len > 0)
		{
			docwire_log(debug) << "Additional formatting blocks found, size " << after_text_block_len << " bytes.";
		}
		std::string dest;
		std::vector<unsigned char>::const_iterator s = *src;
		int char_count = 0;
		for (int i = 0; i < count; i++, s += char_size, record_pos += char_size)
		{
			if (s >= src_end)
			{
				emit_message(make_error_ptr("Unexpected end of buffer."));
				*src = src_end;
				return dest;
			}
			if (record_pos > record_sizes[record_index])
				emit_message(make_error_ptr("Record boundary crossed.", record_pos, record_sizes[record_index]));
			if (record_pos == record_sizes[record_index])
			{
				docwire_log(debug) << "Record boundary reached.";
				record_index++;
				record_pos = 0;
				// At the beginning of each CONTINUE record the option flags byte is repeated.
				// Only the character size flag will be set in this flags byte, the Rich-Text flag and the Far-East flag are set to zero.
				// In each CONTINUE record it is possible that the character size changes
				// from 8‑bit characters to 16‑bit characters and vice versa.
				if (s >= src_end)
				{
					emit_message(make_error_ptr("Unexpected end of buffer."));
					*src = src_end;
					return dest;
				}
				if ((*s) != 0 && (*s) != 1)
					emit_message(make_error_ptr("Incorrect XLUnicodeString flag.", *s));
				char_size = ((*s) & 0x01) ? 2 : 1;
				if (char_size == 2)
				{
					s--;
					record_pos--;
				}
				i--;
				continue;
			}
			if (char_size == 2)
			{
				if (src_end - *src < 2)
				{
					emit_message(make_error_ptr("Unexpected end of buffer."));
					*src = src_end;
					return dest;
				}
				unsigned int uc = getU16LittleEndian(s);
				// warning TODO: Find explanation (documentation) of NULL characters (OO skips them).
				if (uc == 0)
					continue;
				if (utf16_unichar_has_4_bytes(uc))
				{
					record_pos += 2;
					s += 2;
					if (src_end - *src < 2)
					{
						emit_message(make_error_ptr("Unexpected end of buffer."));
						*src = src_end;
						return dest;
					}
					uc = (uc << 16) | getU16LittleEndian(s);
				}
				dest += unichar_to_utf8(uc);
				char_count++;
			}
			else
			{
				if (s >= src_end)
				{
					emit_message(make_error_ptr("Unexpected end of buffer."));
					*src = src_end;
					return dest;
				}
				std::string c2(1, *s);
				if (m_context_stack.top().m_codepage != "ASCII")
				{
					std::lock_guard<std::mutex> xls_converter_mutex_lock(xls_converter_mutex);
					TextConverter tc(m_context_stack.top().m_codepage);
					dest += ustring_to_string(tc.convert(c2));
				}
				else
					dest += c2;
				char_count++;
			}
		}
		*src = s + after_text_block_len;
		record_pos += after_text_block_len;
		return dest;
	}

	void parseSharedStringTable(const std::vector<unsigned char>& sst_buf)
	{
		docwire_log(debug) << "Parsing shared string table.";
		if (sst_buf.size() < 8)
		{
			emit_message(make_error_ptr("Error while parsing shared string table. Buffer must contain at least 8 bytes.", sst_buf.size()));
			return;
		}
		int sst_size = getS32LittleEndian(sst_buf.begin() + 4);
		std::vector<unsigned char>::const_iterator src = sst_buf.begin() + 8;
		size_t record_index = 0;
		size_t record_pos = 8;
		while (src < sst_buf.end() && m_context_stack.top().m_shared_string_table.size() <= sst_size)
		m_context_stack.top().m_shared_string_table.push_back(parseXLUnicodeString(&src, sst_buf.end(), m_context_stack.top().m_shared_string_table_record_sizes, record_index, record_pos));
	}	

	std::string cellText(int row, int col, const std::string& s)
	{
		std::string r;
		while (row > m_context_stack.top().m_last_row)
		{
			r += "\n";
			++m_context_stack.top().m_last_row;
			m_context_stack.top().m_last_col = 0;
		}
		if (col > 0 && col <= m_context_stack.top().m_last_col)
			r += "\t";
		while (col > m_context_stack.top().m_last_col)
		{
			r += "\t";
			++m_context_stack.top().m_last_col;
		}
		r += s;
		return r;
	}

	void processRecord(int rec_type, const std::vector<unsigned char>& rec, std::string& text)
	{
		docwire_log(debug) << hex() << "record" << rec_type;
		if (rec_type != XLS_CONTINUE && m_context_stack.top().m_prev_rec_type == XLS_SST)
			parseSharedStringTable(m_context_stack.top().m_shared_string_table_buf);
		switch (rec_type)
		{
			case XLS_BLANK:
			{
				if (rec.size() < 4)
				{
					emit_message(make_error_ptr("Record is too short. XLS_BLANK must be 4 bytes in length", rec.size()));
					break;
				}
				int row = getU16LittleEndian(rec.begin());
				int col = getU16LittleEndian(rec.begin() + 2);
				text += cellText(row, col, "");
				break;
			}
			case XLS_BOF:
			{
				m_context_stack.top().m_last_row = 0;
				m_context_stack.top().m_last_col = 0;
				// warning TODO: Check for stream type, ignore charts, or make it configurable
				// warning TODO: Mark beginning of sheet (configurable)
				break;
			}
			case XLS_CODEPAGE:
			{
				if (rec.size() == 2)
				{
					int codepage = getU16LittleEndian(rec.begin());
					if (codepage == 1200)
						break;
					else if (codepage == 367)
						m_context_stack.top().m_codepage = "ASCII";
					else
						m_context_stack.top().m_codepage = "cp" + int2string(codepage);
				}
				break;
			}
			case XLS_CONTINUE:
			{
				if (m_context_stack.top().m_prev_rec_type != XLS_SST)
					return; // do not change m_prev_rec_type
				m_context_stack.top().m_shared_string_table_buf.reserve(m_context_stack.top().m_shared_string_table_buf.size() + rec.size());
				m_context_stack.top().m_shared_string_table_buf.insert(m_context_stack.top().m_shared_string_table_buf.end(), rec.begin(), rec.begin() + rec.size());
				m_context_stack.top().m_shared_string_table_record_sizes.push_back(rec.size());
				docwire_log(debug) << "XLS_CONTINUE record for XLS_SST found. Index: " << m_context_stack.top().m_shared_string_table_record_sizes.size() - 1 << ", size:" << rec.size() << ".";
				return;
			}
			case XLS_DATE_1904:
				m_context_stack.top().m_date_shift = 24107.0; 
				break;
			case XLS_EOF:
			{
				text += "\n";
				// warning TODO: Mark end of sheet (configurable)
				break;
			}
			case XLS_FILEPASS:
			{
				docwire_log(info) << "XLS file is encrypted.";
				if (rec.size() >= 2)
				{
					U16 encryption_type = getU16LittleEndian(rec.begin());
					if (encryption_type == 0x0000)
						throw make_error(errors::file_encrypted{}, "XOR obfuscation encryption");
					else if (encryption_type == 0x0001 && rec.size() >= 4)
					{
						U16 header_type = getU16LittleEndian(rec.begin() + 2);
						if (header_type == 0x0001)
							throw make_error(errors::file_encrypted{}, "RC4 encryption");
						else if (header_type == 0x0002 || header_type == 0x0003)
							throw make_error(errors::file_encrypted{}, "RC4 CryptoAPI encryption");
						throw make_error(errors::file_encrypted{}, "unknown RC4 encryption");
					}
				}
				throw make_error(errors::file_encrypted{});
			}
			case XLS_FORMAT:
			{
				if (rec.size() < 2)
				{
					emit_message(make_error_ptr("Record is too short. XLS_FORMAT must be 2 bytes in length", rec.size()));
					break;
				}
				int num_format_id = getU16LittleEndian(rec.begin());
				m_context_stack.top().m_defined_num_format_ids.insert(num_format_id);
				break;
			}
			case XLS_FORMULA:
			{
				if (rec.size() < 14)
				{
					emit_message(make_error_ptr("Record is too short. XLS_FORMULA must be 14 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin());
				int col = getU16LittleEndian(rec.begin()+2);
				if (((unsigned char)rec[12] == 0xFF) && (unsigned char)rec[13] == 0xFF)
				{
					if (rec[6] == 0)
					{
						m_context_stack.top().m_last_string_formula_row = row;
						m_context_stack.top().m_last_string_formula_col = col;
					}
					else if (rec[6] == 1)
					{
						// warning TODO: check and test boolean formulas
						text += (rec[8] ? "TRUE" : "FALSE");
					}
					else if (rec[6] == 2)
						text += "ERROR";
				}
				else
				{
					int xf_index=getU16LittleEndian(rec.begin()+4);
					text += cellText(row, col, parseXNum(rec.begin() + 6,xf_index));
				}
				break;
			}
			case XLS_INTEGER_CELL:
			{
				if (rec.size() < 9)
				{
					emit_message(make_error_ptr("Record is too short. XLS_INTEGER_CELL must be 9 bytes in length", rec.size()));
					break;
				}
				int row = getU16LittleEndian(rec.begin());
				int col = getU16LittleEndian(rec.begin()+2);
				text += cellText(row, col, int2string(getU16LittleEndian(rec.begin() + 7)));
				break;
			}
			case XLS_RSTRING:
			case XLS_LABEL:
			{
				if (rec.size() < 6)
				{
					emit_message(make_error_ptr("Record is too short. XLS_LABEL and XLS_RSTRING must be at least 6 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin()); 
				int col = getU16LittleEndian(rec.begin() + 2);
				std::vector<unsigned char>::const_iterator src=rec.begin() + 6;
				std::vector<size_t> sizes;
				sizes.push_back(rec.size() - 6);
				size_t record_index = 0;
				size_t record_pos = 0;
				text += cellText(row, col, parseXLUnicodeString(&src, rec.end(), sizes, record_index, record_pos));
				break;
			}
			case XLS_LABEL_SST:
			{
				if (rec.size() < 8)
				{
					emit_message(make_error_ptr("Record is too short. XLS_LABEL_SST must be at least 8 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin()); 
				int col = getU16LittleEndian(rec.begin() + 2);
				int sst_index = getU16LittleEndian(rec.begin() + 6);
				if (sst_index >= m_context_stack.top().m_shared_string_table.size() || sst_index < 0)
				{
					emit_message(make_error_ptr("Incorrect SST index.", sst_index, m_context_stack.top().m_shared_string_table.size()));
					return;
				}
				else
					text += cellText(row, col, m_context_stack.top().m_shared_string_table[sst_index]);
				break;
			}
			case XLS_MULBLANK:
			{
				if (rec.size() < 4)
				{
					emit_message(make_error_ptr("Record is too short. XLS_MULBLANK must be at least 4 bytes in length", rec.size()));
					break;
				}
				int row = getU16LittleEndian(rec.begin());
				int start_col = getU16LittleEndian(rec.begin() + 2);
				int end_col=getU16LittleEndian(rec.begin() + rec.size() - 2);
				for (int c = start_col; c <= end_col; c++)
					text += cellText(row, c, "");
				break;
			}
			case XLS_MULRK:
			{
				if (rec.size() < 4)
				{
					emit_message(make_error_ptr("Record is too short. XLS_MULRK must be at least 4 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin());
				int start_col = getU16LittleEndian(rec.begin() + 2);
				int end_col = getU16LittleEndian(rec.begin() + rec.size() - 2);
				int min_size = 4 + 6 * (end_col - start_col + 1);
				if (rec.size() < min_size)
				{
					emit_message(make_error_ptr("Record is too short. XLS_MULRK has its minimum size.", min_size, rec.size()));
					break;
				}
				for (int offset = 4, col = start_col; col <= end_col; offset += 6, col++)
				{
					int xf_index = getU16LittleEndian(rec.begin() + offset);
					text += cellText(row, col, parseRkRec(rec.begin() + offset + 2, xf_index));
				}
				break;
			}
			case XLS_NUMBER:
			case 0x03:
			case 0x103:
			case 0x303:
			{
				if (rec.size() < 14)
				{
					emit_message(make_error_ptr("Record is too short. XLS_NUMBER (or record of number 0x03, 0x103, 0x303) must be at least 14 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin());
				int col = getU16LittleEndian(rec.begin() + 2);
				text += cellText(row, col, parseXNum(rec.begin() + 6, getU16LittleEndian(rec.begin() + 4)));
				break;
			}
			case XLS_RK:
			{
				if (rec.size() < 10)
				{
					emit_message(make_error_ptr("Record is too short. XLS_RK must be at least 10 bytes in length", rec.size()));
					break;
				}
				m_context_stack.top().m_last_string_formula_row = -1;
				int row = getU16LittleEndian(rec.begin());
				int col = getU16LittleEndian(rec.begin() + 2);
				int xf_index = getU16LittleEndian(rec.begin() + 4);
				text += cellText(row, col, parseRkRec(rec.begin() + 6, xf_index));
				break;
			}
			case XLS_SST:
			{
				m_context_stack.top().m_shared_string_table_buf.clear();
				m_context_stack.top().m_shared_string_table_record_sizes.clear();
				m_context_stack.top().m_shared_string_table.clear();
				m_context_stack.top().m_shared_string_table_buf.reserve(rec.size());
				m_context_stack.top().m_shared_string_table_buf.insert(m_context_stack.top().m_shared_string_table_buf.end(), rec.begin(), rec.begin() + rec.size());
				m_context_stack.top().m_shared_string_table_record_sizes.push_back(rec.size());
				break;
			}
			case XLS_STRING:
			{
				std::vector<unsigned char>::const_iterator src = rec.begin();
				if (m_context_stack.top().m_last_string_formula_row < 0) {
					emit_message(make_error_ptr("String record without preceeding string formula."));
					break;
				}
				std::vector<size_t> sizes;
				sizes.push_back(rec.size());
				size_t record_index = 0;
				size_t record_pos = 0;
				text += cellText(m_context_stack.top().m_last_string_formula_row, m_context_stack.top().m_last_string_formula_col, parseXLUnicodeString(&src, rec.end(), sizes, record_index, record_pos));
				break;
			}
			case XLS_XF:
			case 0x43:
			{
				if (rec.size() < 4)
				{
					emit_message(make_error_ptr("Record is too short. XLS_XF (or record of number 0x43) must be at least 4 bytes in length", rec.size()));
					break;
				}
				XFRecord xf_record;
				xf_record.num_format_id = getU16LittleEndian(rec.begin() + 2);
				m_context_stack.top().m_xf_records.push_back(xf_record);
					break;
			} 
		}
		m_context_stack.top().m_prev_rec_type = rec_type;
	}  

	void parseXLS(ThreadSafeOLEStreamReader& reader, std::string& text)
	{
		m_context_stack.top().m_xf_records.clear();
		m_context_stack.top().m_date_shift = 25569.0;
		m_context_stack.top().m_shared_string_table.clear();
		m_context_stack.top().m_shared_string_table_buf.clear();
		m_context_stack.top().m_shared_string_table_record_sizes.clear();
		m_context_stack.top().m_prev_rec_type = 0;
		int m_last_string_formula_row = -1;
		int m_last_string_formula_col = -1;
		m_context_stack.top().m_defined_num_format_ids.clear();
		m_context_stack.top().m_last_row = 0;
		m_context_stack.top().m_last_col = 0;

		std::vector<unsigned char> rec;
		bool read_status = true;
		while (read_status)
		{
			throw_if (oleEof(reader), "BOF record not found", errors::uninterpretable_data{});
			U16 rec_type, rec_len;
			throw_if (!reader.readU16(rec_type) || !reader.readU16(rec_len), reader.getLastError());
			enum BofRecordTypes
			{
				BOF_BIFF_2 = 0x009,
				BOF_BIFF_3 = 0x209,
				BOF_BIFF_4 = 0x0409,
				BOF_BIFF_5_AND_8 = XLS_BOF
			};
			if (rec_type == BOF_BIFF_2 || rec_type == BOF_BIFF_3 || rec_type == BOF_BIFF_4 || rec_type == BOF_BIFF_5_AND_8)
			{
				int bof_struct_size;
				// warning Has all BOF records size 8 or 16?
				if (rec_len == 8 || rec_len == 16)
				{
					switch (rec_type)
					{
						case BOF_BIFF_5_AND_8:
						{
							U16 biff_ver, data_type;
							throw_if (!reader.readU16(biff_ver) || !reader.readU16(data_type), reader.getLastError());
							//On microsoft site there is documentation only for "BIFF8". Documentation from OpenOffice is better:
							/*
								BIFF5:
								Offset	Size	Contents
								0		2		BIFF version (always 0500H for BIFF5). ...
								2		2		Type of the following data:	0005H = Workbook globals
																			0006H = Visual Basic module
																			0010H = Sheet or dialogue
																			0020H = Chart
																			0040H = Macro sheet
																			0100H = Workspace (BIFF5W only)
								4		2		Build identifier, must not be 0
								6		2		Build year
							*/
							/*
								BIFF8:
								Offset	Size	Contents
								0		2		BIFF version (always 0600H for BIFF8)
								2		2		Type of the following data: 0005H = Workbook globals
																			0006H = Visual Basic module
																			0010H = Sheet or dialogue
																			0020H = Chart
																			0040H = Macro sheet
																			0100H = Workspace (BIFF8W only)
								4		2		Build identifier, must not be 0
								6		2		Build year, must not be 0
								8		4		File history flags
								12		4		Lowest Excel version that can read all records in this file
							*/
							if(biff_ver == 0x600)
							{
								docwire_log(debug) << "Detected BIFF8 version";
								rec.resize(8);
								read_status = reader.read(&*rec.begin(), 8);
								m_context_stack.top().m_biff_version = BIFF8;
								bof_struct_size = 16;
							}
							else
							{
								docwire_log(debug) << "Detected BIFF5 version";
								m_context_stack.top().m_biff_version = BIFF5;
								bof_struct_size = 8;
							}
							break;
						}
						case BOF_BIFF_3:
							docwire_log(debug) << "Detected BIFF3 version";
							m_context_stack.top().m_biff_version = BIFF3;
							bof_struct_size = 6;
							break;
						case BOF_BIFF_4:
							docwire_log(debug) << "Detected BIFF4 version";
							m_context_stack.top().m_biff_version = BIFF4;
							bof_struct_size = 6;
							break;
						default:
							docwire_log(debug) << "Detected BIFF2 version";
							m_context_stack.top().m_biff_version = BIFF2;
							bof_struct_size = 4;
					}
					rec.resize(rec_len - (bof_struct_size - 4));
					read_status = reader.read(&*rec.begin(), rec_len - (bof_struct_size - 4));
					break;
				}
				else
					throw_if (rec_len != 8 && rec_len != 16, "Invalid BOF record size", rec_len, errors::uninterpretable_data{});
			}
			else
			{
				rec.resize(126);
				throw_if (!reader.read(&*rec.begin(), 126), reader.getLastError());
			}
		}
		throw_if (oleEof(reader), "BOF record not found", errors::uninterpretable_data{});
		bool eof_rec_found = false;
		while (read_status)
		{
			U16 rec_type;
			try
			{
				throw_if (!reader.readU16(rec_type), reader.getLastError());
			}
			catch (const std::exception&)
			{
				if (text.length() == 0)
					std::throw_with_nested(make_error("Type of record could not be read"));
				else
					emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Type of record could not be read")));
				break;
			}
			if (oleEof(reader))
			{
				processRecord(XLS_EOF, std::vector<unsigned char>(), text);
				return;
			}
			U16 rec_len;
			try
			{
				throw_if (!reader.readU16(rec_len), reader.getLastError());
			}
			catch (const std::exception&)
			{
				if (text.length() == 0)
					std::throw_with_nested(make_error("Length of record could not be read"));
				else
					emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Length of record could not be read")));
				break;
			}
			if (rec_len > 0)
			{
				rec.resize(rec_len);
				read_status = reader.read(&*rec.begin(), rec_len);
				if (!read_status)
					emit_message(make_error_ptr("Error while reading next record", reader.getLastError()));
			}
			else
				rec.clear();
			if (eof_rec_found)
			{
				if (rec_type != XLS_BOF)
					break;
			}
			processRecord(rec_type, rec, text);
			if (rec_type == XLS_EOF)
				eof_rec_found = true;
			else
				eof_rec_found = false;	
		}
	}
};

XLSParser::XLSParser()
{
}

void pimpl_impl<XLSParser>::parse(const data_source& data, const message_callbacks& emit_message)
{
	auto storage = std::make_unique<ThreadSafeOLEStorage>(data.span());
	throw_if (!storage->isValid(), storage->getLastError());
	emit_message(document::Document
		{
			.metadata = [this, emit_message, &storage]()
			{
				attributes::Metadata meta;
				parse_oshared_summary_info(*storage, meta, [emit_message](std::exception_ptr e) { emit_message(std::move(e)); });
				return meta;
			}
		});
	emit_message(document::Text{.text = parse(*storage, emit_message)});
	emit_message(document::CloseDocument{});
}

std::string pimpl_impl<XLSParser>::parse(ThreadSafeOLEStorage& storage, const message_callbacks& emit_message)
{
	docwire_log(debug) << "Using XLS parser.";
	scoped::stack_push<context> context_guard{m_context_stack, context{.emit_message = emit_message}};
	try
	{
		std::lock_guard<std::mutex> parser_mutex_lock(parser_mutex);
		std::unique_ptr<ThreadSafeOLEStreamReader> workbook_reader { static_cast<ThreadSafeOLEStreamReader*>(storage.createStreamReader("Workbook")) };
		std::string text;
		if (workbook_reader != nullptr)
		{
			parseXLS(*workbook_reader, text);
		}
		else
		{
			std::unique_ptr<ThreadSafeOLEStreamReader> book_reader { static_cast<ThreadSafeOLEStreamReader*>(storage.createStreamReader("Book")) };
			throw_if (book_reader == nullptr, storage.getLastError());
			parseXLS(*book_reader, text);
		}		
		return text;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error processing XLS OLE storage"));
	}
}

continuation XLSParser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));

	auto& data = msg->get<data_source>();
	data.assert_not_encrypted(); // This checks if the data_source itself is encrypted (e.g. encrypted ZIP)

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	impl().parse(data, emit_message);
	return continuation::proceed;
}

std::string XLSParser::parse(ThreadSafeOLEStorage& storage, const message_callbacks& emit_message) // TODO: needs to be removed finally
{
	return impl().parse(storage, emit_message);
}

} // namespace docwire
