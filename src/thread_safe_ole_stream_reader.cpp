/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
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

#include "thread_safe_ole_stream_reader.h"

#include "data_stream.h"
#include <new>

using namespace wvWare;

struct ThreadSafeOLEStreamReader::Implementation
{
	DataStream* m_data_stream{};
	uint64_t m_size{0};
	uint64_t m_position{0};
	uint64_t m_chunk_position{0};
	std::vector<uint64_t> m_sector_positions;
	uint32_t m_sector_size{0};
	uint32_t m_current_sector{0};
	std::string m_error;
	bool m_valid{false};
};

ThreadSafeOLEStreamReader::ThreadSafeOLEStreamReader(ThreadSafeOLEStorage *storage, Stream &stream)
	: AbstractOLEStreamReader((wvWare::AbstractOLEStorage*)storage)
{
	impl = nullptr;
	try
	{
		impl = new Implementation;
		impl->m_data_stream = stream.m_data_stream;
		impl->m_position = 0;
		impl->m_chunk_position = 0;
		impl->m_size = stream.m_size;
		impl->m_sector_positions.assign(stream.m_file_positions.begin(), stream.m_file_positions.end());
		impl->m_sector_size = stream.m_sector_size;
		impl->m_valid = true;
		impl->m_current_sector = 0;
		if (!impl->m_data_stream->open())
		{
			impl->m_error = "Empty file";
			impl->m_valid = false;
			return;
		}
		if (impl->m_sector_positions.empty())
		{
			impl->m_error = "Stream is empty";
			impl->m_valid = false;
			return;
		}
		if (!impl->m_data_stream->seek(impl->m_sector_positions[0], SEEK_SET))
		{
			impl->m_error = "Cant seek to the first sector";
			impl->m_valid = false;
		}
	}
	catch (std::bad_alloc& ba)
	{
		if (impl != nullptr)
		{
			delete impl->m_data_stream;
			delete impl;
		}
		stream.m_data_stream = nullptr;
		throw;
	}
}

ThreadSafeOLEStreamReader::~ThreadSafeOLEStreamReader()
{
	if (impl)
	{
		impl->m_data_stream->close();
		delete impl->m_data_stream;
		delete impl;
	}
}

std::string ThreadSafeOLEStreamReader::getLastError() const
{
	return impl->m_error;
}

bool ThreadSafeOLEStreamReader::isValid() const
{
	return impl->m_valid;
}

int ThreadSafeOLEStreamReader::tell() const
{
	return impl->m_position;
}

size_t ThreadSafeOLEStreamReader::size() const
{
	return impl->m_size;
}

bool ThreadSafeOLEStreamReader::read(U8* buf, size_t length)
{
	if (!impl->m_valid)
		return false;
	uint64_t to_read = length;
	uint64_t read_pos = 0;
	if (to_read > impl->m_size - impl->m_position)
	{
		impl->m_error = "Requested size to read is too big";
		to_read = impl->m_size - impl->m_position;
	}
	while (to_read > 0)
	{
		if (to_read <= impl->m_sector_size - impl->m_chunk_position)
		{
			if (!impl->m_data_stream->read(buf + read_pos, sizeof(uint8_t), to_read))
			{
				impl->m_valid = false;
				impl->m_error = "Read past EOF";
				return false;
			}
			impl->m_position += to_read;
			impl->m_chunk_position += to_read;
			to_read = 0;
		}
		else
		{
			uint32_t rest = impl->m_sector_size - impl->m_chunk_position;
			if (rest > 0)
			{
				if (!impl->m_data_stream->read(buf + read_pos, sizeof(uint8_t), rest))
				{
					impl->m_valid = false;
					impl->m_error = "Read past EOF";
					return false;
				}
				read_pos += rest;
				impl->m_position += rest;
				to_read -= rest;
			}
			++impl->m_current_sector;
			if (impl->m_current_sector >= impl->m_sector_positions.size())
			{
				impl->m_valid = false;
				impl->m_error = "Read past EOF";
				return false;
			}
			if (!impl->m_data_stream->seek(impl->m_sector_positions[impl->m_current_sector], SEEK_SET))
			{
				impl->m_error = "Cant seek to the next sector";
				impl->m_valid = false;
				return false;
			}
			impl->m_chunk_position = 0;
		}
	}
	return true;
}

bool ThreadSafeOLEStreamReader::seek(int offset, int whence)
{
	uint64_t new_position;
	if (!impl->m_valid)
		return false;
	if (whence == SEEK_SET)
		new_position = offset;
	else if (whence == SEEK_CUR)
		new_position = impl->m_position + offset;
	else if (whence == SEEK_END)
		new_position = impl->m_size - offset;
	else
	{
		impl->m_error = "Wrong seek type";
		return false;
	}
	if (new_position > impl->m_size)
	{
		impl->m_error = "New position is beyond stream size";
		return false;
	}
	impl->m_position = new_position;
	impl->m_current_sector = new_position / impl->m_sector_size;
	impl->m_chunk_position = new_position - impl->m_current_sector * impl->m_sector_size;
	if (impl->m_current_sector >= impl->m_sector_positions.size())
	{
		impl->m_valid = false;
		impl->m_error = "Read past EOF";
		return false;
	}
	if (!impl->m_data_stream->seek(impl->m_sector_positions[impl->m_current_sector] + impl->m_chunk_position, SEEK_SET))
	{
		impl->m_error = "Cant seek to the selected position";
		impl->m_valid = false;
		return false;
	}
	return true;
}

bool ThreadSafeOLEStreamReader::readU8(U8& data)
{
	return read(&data, 1);
}

U8 ThreadSafeOLEStreamReader::readU8()
{
	U8 data = 0;
	if (!read(&data, 1))
		return 0;
	return data;
}

bool ThreadSafeOLEStreamReader::readS8(S8& data)
{
	return read((U8*)&data, 1);
}

S8 ThreadSafeOLEStreamReader::readS8()
{
	U8 data = 0;
	if (!read(&data, 1))
		return 0;
	return (S8)data;
}

bool ThreadSafeOLEStreamReader::readU16(U16& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read((U8*)data + 1, 1) && read((U8*)data, 1);
	#else
	return read((U8*)&data, 2);
	#endif
}

U16 ThreadSafeOLEStreamReader::readU16()
{
	U16 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read((U8*)&data + 1, 1) || !read((U8*)&data, 1))
	#else
	if (!read((U8*)&data, 2))
	#endif
		return 0;
	return data;
}

bool ThreadSafeOLEStreamReader::readS16(S16& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read((U8*)&data + 1, 1) && read((U8*)&data, 1);
	#else
	return read((U8*)&data, 2);
	#endif
}

S16 ThreadSafeOLEStreamReader::readS16()
{
	U16 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read((U8*)&data + 1, 1) || !read((U8*)&data, 1))
	#else
	if (!read((U8*)&data, 2))
	#endif
		return 0;
	return (S16)data;
}

bool ThreadSafeOLEStreamReader::readU32(U32& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read((U8*)&data + 3, 1) && read((U8*)&data + 2, 1) && read((U8*)&data + 1, 1) && read((U8*)&data, 1);
	#else
	return read((U8*)&data, 4);
	#endif
}

U32 ThreadSafeOLEStreamReader::readU32()
{
	U32 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read((U8*)&data + 3, 1) || !read((U8*)&data + 2, 1) || !read((U8*)&data + 1, 1) || !read((U8*)&data, 1))
	#else
	if (!read((U8*)&data, 4))
	#endif
		return 0;
	return data;
}

bool ThreadSafeOLEStreamReader::readS32(S32& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read((U8*)&data + 3, 1) && read((U8*)&data + 2, 1) && read((U8*)&data + 1, 1) && read((U8*)&data, 1);
	#else
	return read((U8*)&data, 4);
	#endif
}

S32 ThreadSafeOLEStreamReader::readS32()
{
	U32 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read((U8*)&data + 3, 1) || !read((U8*)&data + 2, 1) || !read((U8*)&data + 1, 1) || !read((U8*)&data, 1))
	#else
	if (!read((U8*)&data, 4))
	#endif
		return 0;
	return (S32)data;
}
