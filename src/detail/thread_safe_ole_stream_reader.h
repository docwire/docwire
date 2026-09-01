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

#ifndef DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H
#define DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H

#include "core_export.h"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "data_stream.h"
#include "wv2/src/olestream.h"

namespace docwire
{

class data_stream;
class thread_safe_ole_storage;

class thread_safe_ole_stream_reader : public wvWare::OLEStreamReader
{
	friend class thread_safe_ole_storage;
public:
	struct stream
	{
		uint64_t m_size;
		std::vector<uint32_t> m_file_positions;
		uint32_t m_sector_size;
		data_stream* m_data_stream;
	};
	thread_safe_ole_stream_reader(thread_safe_ole_storage* storage, stream& stream);
	~thread_safe_ole_stream_reader() override;
	std::string getLastError() const;
	bool isValid() const override;
	int tell() const override;
	size_t size() const override;
	bool seek(int offset, int whence = SEEK_SET) override;
	bool readU8(wvWare::U8& data);
	wvWare::U8 readU8() override;
	bool readS8(wvWare::S8& data);
	wvWare::S8 readS8() override;
	bool readU16(wvWare::U16& data);
	wvWare::U16 readU16() override;
	bool readS16(wvWare::S16& data);
	wvWare::S16 readS16() override;
	bool readU32(wvWare::U32& data);
	wvWare::U32 readU32() override;
	bool readS32(wvWare::S32& data);
	wvWare::S32 readS32() override;
	bool read(wvWare::U8* buffer, size_t length) override;
private:
	data_stream* m_data_stream{};
	uint64_t m_size{0};
	uint64_t m_position{0};
	uint64_t m_chunk_position{0};
	std::vector<uint64_t> m_sector_positions;
	uint32_t m_sector_size{0};
	uint32_t m_current_sector{0};
	std::string m_error;
	bool m_valid{false};
};

inline thread_safe_ole_stream_reader::thread_safe_ole_stream_reader(thread_safe_ole_storage* storage, stream& stream)
	: wvWare::OLEStreamReader((wvWare::OLEStorage*)storage)
{
	m_data_stream = stream.m_data_stream;
	m_position = 0;
	m_chunk_position = 0;
	m_size = stream.m_size;
	m_sector_positions.assign(stream.m_file_positions.begin(), stream.m_file_positions.end());
	m_sector_size = stream.m_sector_size;
	m_valid = true;
	m_current_sector = 0;
	if (!m_data_stream->open())
	{
		m_error = "Empty file";
		m_valid = false;
		return;
	}
	if (m_sector_positions.empty())
	{
		m_error = "Stream is empty";
		m_valid = false;
		return;
	}
	if (!m_data_stream->seek(m_sector_positions[0], SEEK_SET))
	{
		m_error = "Cant seek to the first sector";
		m_valid = false;
	}
}

inline thread_safe_ole_stream_reader::~thread_safe_ole_stream_reader()
{
	m_data_stream->close();
	delete m_data_stream;
}

inline std::string thread_safe_ole_stream_reader::getLastError() const
{
	return m_error;
}

inline bool thread_safe_ole_stream_reader::isValid() const
{
	return m_valid;
}

inline int thread_safe_ole_stream_reader::tell() const
{
	return static_cast<int>(m_position);
}

inline size_t thread_safe_ole_stream_reader::size() const
{
	return static_cast<size_t>(m_size);
}

inline bool thread_safe_ole_stream_reader::read(wvWare::U8* buf, size_t length)
{
	if (!m_valid)
		return false;
	uint64_t to_read = length;
	uint64_t read_pos = 0;
	if (to_read > m_size - m_position)
	{
		m_error = "Requested size to read is too big";
		to_read = m_size - m_position;
	}
	while (to_read > 0)
	{
		if (to_read <= m_sector_size - m_chunk_position)
		{
			if (!m_data_stream->read(buf + read_pos, sizeof(uint8_t), to_read))
			{
				m_valid = false;
				m_error = "Read past EOF";
				return false;
			}
			m_position += to_read;
			m_chunk_position += to_read;
			to_read = 0;
		}
		else
		{
			uint32_t rest = m_sector_size - m_chunk_position;
			if (rest > 0)
			{
				if (!m_data_stream->read(buf + read_pos, sizeof(uint8_t), rest))
				{
					m_valid = false;
					m_error = "Read past EOF";
					return false;
				}
				read_pos += rest;
				m_position += rest;
				to_read -= rest;
			}
			++m_current_sector;
			if (m_current_sector >= m_sector_positions.size())
			{
				m_valid = false;
				m_error = "Read past EOF";
				return false;
			}
			if (!m_data_stream->seek(m_sector_positions[m_current_sector], SEEK_SET))
			{
				m_error = "Cant seek to the next sector";
				m_valid = false;
				return false;
			}
			m_chunk_position = 0;
		}
	}
	return true;
}

inline bool thread_safe_ole_stream_reader::seek(int offset, int whence)
{
	uint64_t new_position;
	if (!m_valid)
		return false;
	if (whence == SEEK_SET)
		new_position = offset;
	else if (whence == SEEK_CUR)
		new_position = m_position + offset;
	else if (whence == SEEK_END)
		new_position = m_size - offset;
	else
	{
		m_error = "Wrong seek type";
		return false;
	}
	if (new_position > m_size)
	{
		m_error = "New position is beyond stream size";
		return false;
	}
	m_position = new_position;
	m_current_sector = new_position / m_sector_size;
	m_chunk_position = new_position - m_current_sector * m_sector_size;
	if (m_current_sector >= m_sector_positions.size())
	{
		m_valid = false;
		m_error = "Read past EOF";
		return false;
	}
	if (!m_data_stream->seek(m_sector_positions[m_current_sector] + m_chunk_position, SEEK_SET))
	{
		m_error = "Cant seek to the selected position";
		m_valid = false;
		return false;
	}
	return true;
}

inline bool thread_safe_ole_stream_reader::readU8(wvWare::U8& data)
{
	return read(&data, 1);
}

inline wvWare::U8 thread_safe_ole_stream_reader::readU8()
{
	wvWare::U8 data = 0;
	if (!read(&data, 1))
		return 0;
	return data;
}

inline bool thread_safe_ole_stream_reader::readS8(wvWare::S8& data)
{
	return read(reinterpret_cast<wvWare::U8*>(&data), 1);
}

inline wvWare::S8 thread_safe_ole_stream_reader::readS8()
{
	wvWare::U8 data = 0;
	if (!read(&data, 1))
		return 0;
	return static_cast<wvWare::S8>(data);
}

inline bool thread_safe_ole_stream_reader::readU16(wvWare::U16& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) && read(reinterpret_cast<wvWare::U8*>(&data), 1);
	#else
	return read(reinterpret_cast<wvWare::U8*>(&data), 2);
	#endif
}

inline wvWare::U16 thread_safe_ole_stream_reader::readU16()
{
	wvWare::U16 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) || !read(reinterpret_cast<wvWare::U8*>(&data), 1))
	#else
	if (!read(reinterpret_cast<wvWare::U8*>(&data), 2))
	#endif
		return 0;
	return data;
}

inline bool thread_safe_ole_stream_reader::readS16(wvWare::S16& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) && read(reinterpret_cast<wvWare::U8*>(&data), 1);
	#else
	return read(reinterpret_cast<wvWare::U8*>(&data), 2);
	#endif
}

inline wvWare::S16 thread_safe_ole_stream_reader::readS16()
{
	wvWare::U16 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) || !read(reinterpret_cast<wvWare::U8*>(&data), 1))
	#else
	if (!read(reinterpret_cast<wvWare::U8*>(&data), 2))
	#endif
		return 0;
	return static_cast<wvWare::S16>(data);
}

inline bool thread_safe_ole_stream_reader::readU32(wvWare::U32& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read(reinterpret_cast<wvWare::U8*>(&data) + 3, 1) && read(reinterpret_cast<wvWare::U8*>(&data) + 2, 1) && read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) && read(reinterpret_cast<wvWare::U8*>(&data), 1);
	#else
	return read(reinterpret_cast<wvWare::U8*>(&data), 4);
	#endif
}

inline wvWare::U32 thread_safe_ole_stream_reader::readU32()
{
	wvWare::U32 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read(reinterpret_cast<wvWare::U8*>(&data) + 3, 1) || !read(reinterpret_cast<wvWare::U8*>(&data) + 2, 1) || !read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) || !read(reinterpret_cast<wvWare::U8*>(&data), 1))
	#else
	if (!read(reinterpret_cast<wvWare::U8*>(&data), 4))
	#endif
		return 0;
	return data;
}

inline bool thread_safe_ole_stream_reader::readS32(wvWare::S32& data)
{
	#if defined(WORDS_BIGENDIAN)
	return read(reinterpret_cast<wvWare::U8*>(&data) + 3, 1) && read(reinterpret_cast<wvWare::U8*>(&data) + 2, 1) && read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) && read(reinterpret_cast<wvWare::U8*>(&data), 1);
	#else
	return read(reinterpret_cast<wvWare::U8*>(&data), 4);
	#endif
}

inline wvWare::S32 thread_safe_ole_stream_reader::readS32()
{
	wvWare::U32 data = 0;
	#if defined(WORDS_BIGENDIAN)
	if (!read(reinterpret_cast<wvWare::U8*>(&data) + 3, 1) || !read(reinterpret_cast<wvWare::U8*>(&data) + 2, 1) || !read(reinterpret_cast<wvWare::U8*>(&data) + 1, 1) || !read(reinterpret_cast<wvWare::U8*>(&data), 1))
	#else
	if (!read(reinterpret_cast<wvWare::U8*>(&data), 4))
	#endif
		return 0;
	return static_cast<wvWare::S32>(data);
}

} // namespace docwire

#endif // DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H
