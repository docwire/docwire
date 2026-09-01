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

#ifndef DOCWIRE_DATA_STREAM_H
#define DOCWIRE_DATA_STREAM_H

#include "core_export.h"
#include <cstring>
#include <stdio.h>
#include <string>

namespace docwire
{

class data_stream
{
	public:
		virtual ~data_stream(){}
		virtual bool open() = 0;
		virtual bool close() = 0;
		virtual bool read(void* data, int element_size, size_t elements_num) = 0;
		virtual bool seek(int offset, int whence) = 0;
		virtual bool eof() = 0;
		virtual int getc() = 0;
		virtual bool unGetc(int ch) = 0;
		virtual size_t size() = 0;
		virtual size_t tell() = 0;
		virtual std::string name() = 0;
		virtual data_stream* clone() = 0;
};

class file_stream : public data_stream
{
	public:
		file_stream(const std::string& file_name);
		~file_stream();
		bool open();
		bool close();
		bool read(void* data, int element_size, size_t elements_num);
		bool seek(int offset, int whence);
		bool eof();
		int getc();
		bool unGetc(int ch);
		size_t size();
		size_t tell();
		std::string name();
		data_stream* clone();

	private:
		FILE* m_file;
		std::string m_file_name;
		bool m_opened;
};

class buffer_stream : public data_stream
{
	public:
		buffer_stream(const char* buffer, size_t size);
		bool open();
		bool close();
		bool read(void* data, int element_size, size_t elements_num);
		bool seek(int offset, int whence);
		bool eof();
		int getc();
		bool unGetc(int ch);
		size_t size();
		size_t tell();
		std::string name();
		data_stream* clone();

	private:
		const char* m_buffer;
		size_t m_size;
		size_t m_pointer;
};

inline file_stream::file_stream(const std::string& file_name)
{
	m_file = nullptr;
	m_opened = false;
	m_file_name = file_name;
}

inline file_stream::~file_stream()
{
	if (m_file)
		fclose(m_file);
}

inline bool file_stream::open()
{
	if (m_opened)
		return true;
	m_file = fopen(m_file_name.c_str(), "rb");
	if (m_file != nullptr)
		m_opened = true;
	return m_opened;
}

inline bool file_stream::close()
{
	if (!m_opened)
		return true;
	if (m_file)
		fclose(m_file);
	m_opened = false;
	m_file = nullptr;
	return true;
}

inline bool file_stream::read(void* data, int element_size, size_t elements_num)
{
	if (!m_opened)
		return false;
	if (fread(data, element_size, elements_num, m_file) != elements_num)
		return false;
	return true;
}

inline bool file_stream::seek(int offset, int whence)
{
	if (!m_opened)
		return false;
	if (fseek(m_file, offset, whence) != 0)
		return false;
	return true;
}

inline bool file_stream::eof()
{
	if (!m_opened)
		return true;
	return !(feof(m_file) == 0);
}

inline int file_stream::getc()
{
	if (!m_opened)
		return 0;
	return fgetc(m_file);
}

inline bool file_stream::unGetc(int ch)
{
	if (!m_opened)
		return false;
	return ungetc(ch, m_file) == ch;
}

inline size_t file_stream::size()
{
	if (!m_opened)
		return 0;
	size_t current = ftell(m_file);
	if (fseek(m_file, 0, SEEK_END) != 0)
		return 0;
	size_t size = ftell(m_file);
	fseek(m_file, current, SEEK_SET);
	return size;
}

inline size_t file_stream::tell()
{
	return ftell(m_file);
}

inline std::string file_stream::name()
{
	return m_file_name;
}

inline data_stream* file_stream::clone()
{
	return new file_stream(m_file_name);
}

inline buffer_stream::buffer_stream(const char *buffer, size_t size)
{
	m_buffer = buffer;
	m_size = size;
	m_pointer = 0;
}

inline bool buffer_stream::open()
{
	m_pointer = 0;
	return true;
}

inline bool buffer_stream::close()
{
	return true;
}

inline bool buffer_stream::read(void *data, int element_size, size_t elements_num)
{
	size_t len = element_size * elements_num;
	if (len > m_size - m_pointer)
		return false;
	memcpy(data, m_buffer + m_pointer, len);
	m_pointer += len;
	return true;
}

inline bool buffer_stream::seek(int offset, int whence)
{
	size_t position;
	switch (whence)
	{
		case SEEK_SET:
			position = offset;
			break;
		case SEEK_CUR:
			position = m_pointer + offset;
			break;
		case SEEK_END:
			position = m_size + offset;
			break;
		default:
			return false;
	}
	if (position > m_size)
		return false;
	m_pointer = position;
	return true;
}

inline bool buffer_stream::eof()
{
	return m_pointer == m_size;
}

inline int buffer_stream::getc()
{
	if (m_size - m_pointer < 1)
		return EOF;
	return m_buffer[m_pointer++];
}

inline bool buffer_stream::unGetc(int ch)
{
	if (m_pointer < 1)
	{
		return false;
	}
	--m_pointer;
	return true;
}

inline size_t buffer_stream::size()
{
	return m_size;
}

inline size_t buffer_stream::tell()
{
	return m_pointer;
}

inline std::string buffer_stream::name()
{
	return "Memory buffer";
}

inline data_stream* buffer_stream::clone()
{
	return new buffer_stream(m_buffer, m_size);
}

} // namespace docwire

#endif	//DOCWIRE_DATA_STREAM_H
