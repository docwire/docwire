/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include "data_stream.h"

#include <new>
#include <string.h>

struct FileStream::Implementation
{
	FILE* m_file;
	std::string m_file_name;
	bool m_opened;
};

FileStream::FileStream(const std::string& file_name)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_file = NULL;
		impl->m_opened = false;
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

FileStream::~FileStream()
{
	if (impl)
	{
		if (impl->m_file)
			fclose(impl->m_file);
		delete impl;
	}
}

bool FileStream::open()
{
	if (impl->m_opened)
		return true;
	impl->m_file = fopen(impl->m_file_name.c_str(), "rb");
	if (impl->m_file != NULL)
		impl->m_opened = true;
	return impl->m_opened;
}

bool FileStream::close()
{
	if (!impl->m_opened)
		return true;
	if (impl->m_file)
		fclose(impl->m_file);
	impl->m_opened = false;
	impl->m_file = NULL;
	return true;
}

bool FileStream::read(void* data, int element_size, size_t elements_num)
{
	if (!impl->m_opened)
		return false;
	if (fread(data, element_size, elements_num, impl->m_file) != elements_num)
		return false;
	return true;
}

bool FileStream::seek(int offset, int whence)
{
	if (!impl->m_opened)
		return false;
	if (fseek(impl->m_file, offset, whence) != 0)
		return false;
	return true;
}

bool FileStream::eof()
{
	if (!impl->m_opened)
		return true;
	return !(feof(impl->m_file) == 0);
}

int FileStream::getc()
{
	if (!impl->m_opened)
		return 0;
	return fgetc(impl->m_file);
}

bool FileStream::unGetc(int ch)
{
	if (!impl->m_opened)
		return false;
	return ungetc(ch, impl->m_file) == ch;
}

size_t FileStream::size()
{
	if (!impl->m_opened)
		return 0;
	size_t current = ftell(impl->m_file);
	if (fseek(impl->m_file, 0, SEEK_END) != 0)
		return 0;
	size_t size = ftell(impl->m_file);
	fseek(impl->m_file, current, SEEK_SET);
	return size;
}

size_t FileStream::tell()
{
	return ftell(impl->m_file);
}

std::string FileStream::name()
{
	return impl->m_file_name;
}

DataStream* FileStream::clone()
{
	return new FileStream(impl->m_file_name);
}

struct BufferStream::Implementation
{
	const char* m_buffer;
	size_t m_size;
	size_t m_pointer;
};

BufferStream::BufferStream(const char *buffer, size_t size)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_buffer = buffer;
		impl->m_size = size;
		impl->m_pointer = 0;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
			delete impl;
		impl = NULL;
		throw;
	}
}

BufferStream::~BufferStream()
{
	if (impl)
		delete impl;
}

bool BufferStream::open()
{
	impl->m_pointer = 0;
	return true;
}

bool BufferStream::close()
{
	return true;
}

bool BufferStream::read(void *data, int element_size, size_t elements_num)
{
	size_t len = element_size * elements_num;
	if (len > impl->m_size - impl->m_pointer)
		return false;
	memcpy(data, impl->m_buffer + impl->m_pointer, len);
	impl->m_pointer += len;
	return true;
}

bool BufferStream::seek(int offset, int whence)
{
	size_t position;
	switch (whence)
	{
		case SEEK_SET:
			position = offset;
			break;
		case SEEK_CUR:
			position = impl->m_pointer + offset;
			break;
		case SEEK_END:
			position = impl->m_size + offset;
			break;
		default:
			return false;
	}
	if (position > impl->m_size)
		return false;
	impl->m_pointer = position;
	return true;
}

bool BufferStream::eof()
{
	return impl->m_pointer == impl->m_size;
}

int BufferStream::getc()
{
	if (impl->m_size - impl->m_pointer < 1)
		return EOF;
	return impl->m_buffer[impl->m_pointer++];
}

bool BufferStream::unGetc(int ch)
{
	if (impl->m_pointer < 1)
	{
		return false;
	}
	--impl->m_pointer;
	return true;
}

size_t BufferStream::size()
{
	return impl->m_size;
}

size_t BufferStream::tell()
{
	return impl->m_pointer;
}

std::string BufferStream::name()
{
	return "Memory buffer";
}

DataStream* BufferStream::clone()
{
	return new BufferStream(impl->m_buffer, impl->m_size);
}
