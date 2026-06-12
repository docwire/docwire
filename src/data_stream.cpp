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

#include "data_stream.h"

#include <string.h>

namespace docwire
{

template<>
struct pimpl_impl<file_stream> : pimpl_impl_base
{
	FILE* m_file;
	std::string m_file_name;
	bool m_opened;
};

file_stream::file_stream(const std::string& file_name)
{
	impl().m_file = NULL;
	impl().m_opened = false;
	impl().m_file_name = file_name;
}

file_stream::~file_stream()
{
	if (impl().m_file)
		fclose(impl().m_file);
}

bool file_stream::open()
{
	if (impl().m_opened)
		return true;
	impl().m_file = fopen(impl().m_file_name.c_str(), "rb");
	if (impl().m_file != NULL)
		impl().m_opened = true;
	return impl().m_opened;
}

bool file_stream::close()
{
	if (!impl().m_opened)
		return true;
	if (impl().m_file)
		fclose(impl().m_file);
	impl().m_opened = false;
	impl().m_file = NULL;
	return true;
}

bool file_stream::read(void* data, int element_size, size_t elements_num)
{
	if (!impl().m_opened)
		return false;
	if (fread(data, element_size, elements_num, impl().m_file) != elements_num)
		return false;
	return true;
}

bool file_stream::seek(int offset, int whence)
{
	if (!impl().m_opened)
		return false;
	if (fseek(impl().m_file, offset, whence) != 0)
		return false;
	return true;
}

bool file_stream::eof()
{
	if (!impl().m_opened)
		return true;
	return !(feof(impl().m_file) == 0);
}

int file_stream::getc()
{
	if (!impl().m_opened)
		return 0;
	return fgetc(impl().m_file);
}

bool file_stream::unGetc(int ch)
{
	if (!impl().m_opened)
		return false;
	return ungetc(ch, impl().m_file) == ch;
}

size_t file_stream::size()
{
	if (!impl().m_opened)
		return 0;
	size_t current = ftell(impl().m_file);
	if (fseek(impl().m_file, 0, SEEK_END) != 0)
		return 0;
	size_t size = ftell(impl().m_file);
	fseek(impl().m_file, current, SEEK_SET);
	return size;
}

size_t file_stream::tell()
{
	return ftell(impl().m_file);
}

std::string file_stream::name()
{
	return impl().m_file_name;
}

data_stream* file_stream::clone()
{
	return new file_stream(impl().m_file_name);
}

template<>
struct pimpl_impl<buffer_stream> : pimpl_impl_base
{
	const char* m_buffer;
	size_t m_size;
	size_t m_pointer;
};

buffer_stream::buffer_stream(const char *buffer, size_t size)
{
	impl().m_buffer = buffer;
	impl().m_size = size;
	impl().m_pointer = 0;
}

bool buffer_stream::open()
{
	impl().m_pointer = 0;
	return true;
}

bool buffer_stream::close()
{
	return true;
}

bool buffer_stream::read(void *data, int element_size, size_t elements_num)
{
	size_t len = element_size * elements_num;
	if (len > impl().m_size - impl().m_pointer)
		return false;
	memcpy(data, impl().m_buffer + impl().m_pointer, len);
	impl().m_pointer += len;
	return true;
}

bool buffer_stream::seek(int offset, int whence)
{
	size_t position;
	switch (whence)
	{
		case SEEK_SET:
			position = offset;
			break;
		case SEEK_CUR:
			position = impl().m_pointer + offset;
			break;
		case SEEK_END:
			position = impl().m_size + offset;
			break;
		default:
			return false;
	}
	if (position > impl().m_size)
		return false;
	impl().m_pointer = position;
	return true;
}

bool buffer_stream::eof()
{
	return impl().m_pointer == impl().m_size;
}

int buffer_stream::getc()
{
	if (impl().m_size - impl().m_pointer < 1)
		return EOF;
	return impl().m_buffer[impl().m_pointer++];
}

bool buffer_stream::unGetc(int ch)
{
	if (impl().m_pointer < 1)
	{
		return false;
	}
	--impl().m_pointer;
	return true;
}

size_t buffer_stream::size()
{
	return impl().m_size;
}

size_t buffer_stream::tell()
{
	return impl().m_pointer;
}

std::string buffer_stream::name()
{
	return "Memory buffer";
}

data_stream* buffer_stream::clone()
{
	return new buffer_stream(impl().m_buffer, impl().m_size);
}

} // namespace docwire
