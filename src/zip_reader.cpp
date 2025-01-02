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

#include "zip_reader.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "throw_if.h"
#include <vector>
#include "unzip.h"

namespace docwire
{

const int CASESENSITIVITY = 1;

//data for reading from buffer (insted of file)
struct ZippedBuffer
{
	std::span<const std::byte> m_span;
	size_t m_pointer;
};

//following static functions will be used for reading from buffer:

static voidpf buffer_open(voidpf opaque, const char* filename, int mode)
{
	return (voidpf)1;	//if NULL, unzip library will consider this function has failed
}

static uLong buffer_read(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	ZippedBuffer* buffer = (ZippedBuffer*)opaque;
	if (buffer->m_span.size() < buffer->m_pointer + size)
	{
		size_t readed = buffer->m_span.size() - buffer->m_pointer;
		memcpy(buf, buffer->m_span.data() + buffer->m_pointer, readed);
		buffer->m_pointer = buffer->m_span.size();
		return readed;
	}
	else
	{
		memcpy(buf, buffer->m_span.data() + buffer->m_pointer, size);
		buffer->m_pointer += size;
		return size;
	}
	return 0;
}

static uLong buffer_write(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	return 0;	//unimplemented, we dont need this
}

static long buffer_tell(voidpf opaque, voidpf stream)
{
	return ((ZippedBuffer*)opaque)->m_pointer;
}

static long buffer_seek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	ZippedBuffer* buffer = (ZippedBuffer*)opaque;
	size_t position;
	switch (origin)
	{
		case ZLIB_FILEFUNC_SEEK_CUR :
			position = buffer->m_pointer + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_END :
			position = buffer->m_span.size() - offset;
			break;
		case ZLIB_FILEFUNC_SEEK_SET :
			position = offset;
			break;
		default:
			return -1;
	}
	if (position > buffer->m_span.size())
		position = buffer->m_span.size();
	buffer->m_pointer = position;
	return 0;
}

static int buffer_close(voidpf opaque, voidpf stream)
{
	return 0;
}

static int buffer_error(voidpf opaque, voidpf stream)
{
	return 0;	//no errors at all?
}

template<>
struct pimpl_impl<ZipReader> : pimpl_impl_base
{
	unzFile ArchiveFile;
	std::map<std::string, unz_file_pos> m_directory;
	bool m_opened_for_chunks;
	std::span<const std::byte> m_span;
	ZippedBuffer* m_zipped_buffer;
};

ZipReader::ZipReader(const data_source& data)
{
		impl().m_opened_for_chunks = false;
		impl().m_span = data.span();
		impl().ArchiveFile = NULL;
		impl().m_zipped_buffer = NULL;
}

ZipReader::~ZipReader()
{
	if (impl().ArchiveFile != NULL)
		unzClose(impl().ArchiveFile);
	if (impl().m_zipped_buffer != NULL)
		delete impl().m_zipped_buffer;
}

void ZipReader::open()
{
		impl().m_zipped_buffer = new ZippedBuffer;
		impl().m_zipped_buffer->m_span = impl().m_span;
		impl().m_zipped_buffer->m_pointer = 0;
		zlib_filefunc_def read_from_buffer_functions;
		read_from_buffer_functions.zopen_file = &buffer_open;
		read_from_buffer_functions.zread_file = &buffer_read;
		read_from_buffer_functions.zwrite_file = &buffer_write;
		read_from_buffer_functions.ztell_file = &buffer_tell;
		read_from_buffer_functions.zseek_file = &buffer_seek;
		read_from_buffer_functions.zclose_file = &buffer_close;
		read_from_buffer_functions.zerror_file = &buffer_error;
		read_from_buffer_functions.opaque = impl().m_zipped_buffer;
		//this function allows us to override default behaviour (reading from hard disc)
		impl().ArchiveFile = unzOpen2("stream", &read_from_buffer_functions);
	throw_if (impl().ArchiveFile == NULL, "Could not open zip archive");
}

bool ZipReader::exists(const std::string& file_name) const
{
	return (unzLocateFile(impl().ArchiveFile, file_name.c_str(), CASESENSITIVITY) == UNZ_OK);
}

bool ZipReader::read(const std::string& file_name, std::string* contents, int num_of_chars)
{
	int res;
	if (impl().m_directory.size() > 0)
	{
		std::map<std::string, unz_file_pos>::iterator i = impl().m_directory.find(file_name);
		if (i == impl().m_directory.end())
			return false;
		res = unzGoToFilePos(impl().ArchiveFile, &i->second);
	}
	else
		res = unzLocateFile(impl().ArchiveFile, file_name.c_str(), CASESENSITIVITY);
	if (res != UNZ_OK)
		return false;
	res = unzOpenCurrentFile(impl().ArchiveFile);
	if (res != UNZ_OK)
		return false;
	*contents = "";
	char buffer[1024 + 1];
	while((res = unzReadCurrentFile(impl().ArchiveFile, buffer, (num_of_chars > 0 && num_of_chars < 1024) ? num_of_chars : 1024)) > 0)
	{
		buffer[res] = '\0';
		*contents += buffer;
		if (num_of_chars > 0)
			if (contents->length() >= num_of_chars)
			{
				*contents = contents->substr(0, num_of_chars);
				break;
			}
	}
	if (res < 0)
	{
		unzCloseCurrentFile(impl().ArchiveFile);
		return false;
	}
	unzCloseCurrentFile(impl().ArchiveFile);
	return true;
}

void ZipReader::closeReadingFileForChunks()
{
	impl().m_opened_for_chunks = false;
}

bool ZipReader::readChunk(const std::string& file_name, char* contents, int num_of_chars, int& readed)
{
	if (impl().m_opened_for_chunks == false)
	{
		int res;
		if (impl().m_directory.size() > 0)
		{
			std::map<std::string, unz_file_pos>::iterator i = impl().m_directory.find(file_name);
			if (i == impl().m_directory.end())
				return false;
			res = unzGoToFilePos(impl().ArchiveFile, &i->second);
		}
		else
			res = unzLocateFile(impl().ArchiveFile, file_name.c_str(), CASESENSITIVITY);
		if (res != UNZ_OK)
			return false;
		res = unzOpenCurrentFile(impl().ArchiveFile);
		if (res != UNZ_OK)
			return false;
		impl().m_opened_for_chunks = true;
	}
	readed = unzReadCurrentFile(impl().ArchiveFile, contents, num_of_chars);
	if (readed < 0)
	{
		unzCloseCurrentFile(impl().ArchiveFile);
		return false;
	}
	if(readed < num_of_chars)	//end of file detected
	{
		contents[readed] = '\0';
		unzCloseCurrentFile(impl().ArchiveFile);
		impl().m_opened_for_chunks = false;
		return true;
	}
	contents[readed] = '\0';
	return true;
}

bool ZipReader::readChunk(const std::string& file_name, std::string* contents, int num_of_chars)
{
	std::vector<char> vcontents(num_of_chars + 1);
	int readed;
	if (!readChunk(file_name, &vcontents[0], num_of_chars, readed))
		return false;
	if (readed == 0)
		*contents = "";
	else
		contents->assign(vcontents.begin(), vcontents.begin() + readed);
	return true;
}

bool ZipReader::getFileSize(const std::string& file_name, unsigned long& file_size)
{
	int res;
	unz_file_info file_info;
	if (impl().m_directory.size() > 0)
	{
		std::map<std::string, unz_file_pos>::iterator i = impl().m_directory.find(file_name);
		if (i == impl().m_directory.end())
			return false;
		res = unzGoToFilePos(impl().ArchiveFile, &i->second);
	}
	else
		res = unzLocateFile(impl().ArchiveFile, file_name.c_str(), CASESENSITIVITY);
	if (res != UNZ_OK)
		return false;
	if (unzGetCurrentFileInfo(impl().ArchiveFile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
		return false;
	file_size = file_info.uncompressed_size;
	return true;
}

bool ZipReader::loadDirectory()
{
	impl().m_directory.clear();
	if (unzGoToFirstFile(impl().ArchiveFile) != UNZ_OK)
		return false;
	for (;;)
	{
		char name[1024];
		if (unzGetCurrentFileInfo(impl().ArchiveFile, NULL, name, 1024, NULL, 0, NULL, 0) != UNZ_OK)
			return false;
		unz_file_pos pos;
		if (unzGetFilePos(impl().ArchiveFile, &pos) != UNZ_OK)
			return false;
		impl().m_directory[name] = pos;
		int res = unzGoToNextFile(impl().ArchiveFile);
		if (res == UNZ_END_OF_LIST_OF_FILE)
			break;
		if (res != UNZ_OK)
			return false;
	}
	return true;
}

}; // namespace docwire
