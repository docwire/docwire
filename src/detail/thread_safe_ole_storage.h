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

#ifndef DOCWIRE_THREAD_SAFE_OLE_STORAGE_H
#define DOCWIRE_THREAD_SAFE_OLE_STORAGE_H

#include "core_export.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>
#include <span>
#include <string>
#include <vector>
#include "data_stream.h"
#include "detail/ustring_conversions.h"
#include "thread_safe_ole_stream_reader.h"
#include "wv2/src/olestorage.h"

namespace docwire
{

class thread_safe_ole_storage : public wvWare::OLEStorage
{
	public:
		explicit thread_safe_ole_storage(const std::string& file_name);
		thread_safe_ole_storage(std::span<const std::byte> buffer);
		~thread_safe_ole_storage() override;
		bool isValid() const override;
		bool open(wvWare::OLEStorage::Mode mode) override;
		void close() override;
		std::string name() const override;
		std::string getLastError();
		bool getStreamsAndStoragesList(std::vector<std::string>& components);
		bool enterDirectory(const std::string& directory_path);
		bool leaveDirectory();
		bool readDirectFromBuffer(unsigned char* buffer, int size, int offset) override;
		wvWare::OLEStreamReader* createStreamReader(const std::string& stream_path) override;
	private:
		void streamDestroyed(wvWare::OLEStream* stream) override;

		struct directory_entry
		{
			std::string m_name;
			enum object_type
			{
				unknown_unallocated = 0x01,
				storage = 0x01,
				stream = 0x02,
				root_storage = 0x05
			};
			object_type m_object_type;
			uint8_t m_color_flag{0};
			uint32_t m_left_sibling{0};
			uint32_t m_right_sibling{0};
			uint32_t m_child{0};
			uint32_t m_start_sector_location{0};
			uint64_t m_stream_size{0};
			bool m_added{false};
		};

		bool m_is_valid_ole;
		std::string m_error;
		std::string m_file_name;
		std::unique_ptr<data_stream> m_data_stream;
		uint16_t m_sector_size{0}, m_mini_sector_size{0};
		uint32_t m_number_of_directories;
		uint16_t m_header_version{0};
		uint32_t m_number_of_fat_sectors{0};
		uint32_t m_first_sector_directory_location{0};
		uint32_t m_first_mini_fat_sector_location{0};
		uint32_t m_number_of_mini_fat_sectors{0};
		uint32_t m_first_difat_sector_location{0};
		uint32_t m_number_of_difat_sectors{0};
		uint32_t m_mini_stream_cut_off{0};
		uint16_t m_byte_order{0};

		std::vector<uint32_t> m_fat_sectors_chain;
		std::vector<uint32_t> m_sectors_chain;
		std::vector<uint32_t> m_mini_sectors_chain;

		std::shared_ptr<directory_entry> m_current_directory;
		std::vector<std::shared_ptr<directory_entry>> m_directories;
		std::vector<std::shared_ptr<directory_entry>> m_child_directories;
		std::vector<std::shared_ptr<directory_entry>> m_inside_directories;
		bool m_child_directories_loaded;

		void init_from_buffer(std::span<const std::byte> buffer);
		void getStreamPositions(std::vector<uint32_t>& stream_positions, bool mini_stream, const std::shared_ptr<directory_entry>& dir_entry);
		bool getCurrentDirectoryChilds();
		void getStoragesAndStreams();
		void getMiniFatSectorChain();
		void getFatSectorChain();
		void getFatArraySectorChain();
		bool skipBytes(int bytes_to_skip);
		bool getUint16(uint16_t& data);
		bool getUint32(uint32_t& data);
		void parseHeader();
};

inline thread_safe_ole_storage::thread_safe_ole_storage(const std::string& file_name)
{
	m_file_name = file_name;
	m_is_valid_ole = true;
	m_data_stream = std::make_unique<file_stream>(file_name);
	if (!m_data_stream->open())
	{
		m_is_valid_ole = false;
		m_error = "File " + file_name + " cannot be open";
	}
	m_child_directories_loaded = false;
	parseHeader();
	getFatArraySectorChain();
	getFatSectorChain();
	getMiniFatSectorChain();
	getStoragesAndStreams();
}

inline void thread_safe_ole_storage::init_from_buffer(std::span<const std::byte> buffer)
{
	m_file_name = "Memory buffer";
	m_is_valid_ole = true;
	m_data_stream = std::make_unique<buffer_stream>(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	if (!m_data_stream->open())
	{
		m_is_valid_ole = false;
		m_error = "Memory buffer cannot be open";
	}
	m_child_directories_loaded = false;
	parseHeader();
	getFatArraySectorChain();
	getFatSectorChain();
	getMiniFatSectorChain();
	getStoragesAndStreams();
}

inline thread_safe_ole_storage::thread_safe_ole_storage(std::span<const std::byte> buffer)
{
	init_from_buffer(buffer);
}

inline thread_safe_ole_storage::~thread_safe_ole_storage() = default;

inline bool thread_safe_ole_storage::open(wvWare::OLEStorage::Mode mode)
{
	if (mode == wvWare::OLEStorage::ReadOnly)
		return true;
	return false;
}

inline bool thread_safe_ole_storage::isValid() const
{
	return m_is_valid_ole;
}

inline void thread_safe_ole_storage::close()
{
}

inline std::string thread_safe_ole_storage::getLastError()
{
	return m_error;
}

inline std::string thread_safe_ole_storage::name() const
{
	return m_file_name;
}

inline bool thread_safe_ole_storage::getStreamsAndStoragesList(std::vector<std::string>& components)
{
	components.clear();
	if (!m_is_valid_ole || m_current_directory == nullptr)
		return false;
	if (!m_child_directories_loaded)
	{
		if (!getCurrentDirectoryChilds())
			return false;
		m_child_directories_loaded = true;
	}
	for (auto& m_child_directory : m_child_directories)
	{
		components.push_back(m_child_directory->m_name);
	}
	return true;
}

inline bool thread_safe_ole_storage::enterDirectory(const std::string& directory_path)
{
	if (!m_is_valid_ole || m_current_directory == nullptr)
		return false;
	if (!m_child_directories_loaded)
	{
		if (!getCurrentDirectoryChilds())
			return false;
		m_child_directories_loaded = true;
	}
	for (size_t i = 0; i < m_child_directories.size(); ++i)
	{
		if (m_child_directories[i]->m_name == directory_path)
		{
			if (m_child_directories[i]->m_object_type != directory_entry::storage)
			{
				m_error = "Specified object is not directory";
				return false;
			}
			m_inside_directories.push_back(m_current_directory);
			m_current_directory = m_child_directories[i];
			m_child_directories_loaded = false;
			for (auto& m_child_directory : m_child_directories)
				m_child_directory->m_added = false;
			m_child_directories.clear();
			return true;
		}
	}
	m_error = "Specified directory does not exist";
	return false;
}

inline bool thread_safe_ole_storage::leaveDirectory()
{
	if (!m_is_valid_ole || m_current_directory == nullptr)
		return false;
	if (m_inside_directories.empty())
	{
		m_error = "Already in root directory";
		return false;
	}
	m_current_directory = m_inside_directories.back();
	m_inside_directories.pop_back();
	m_child_directories_loaded = false;
	for (auto& m_child_directory : m_child_directories)
		m_child_directory->m_added = false;
	m_child_directories.clear();
	return true;
}

inline wvWare::OLEStreamReader* thread_safe_ole_storage::createStreamReader(const std::string& stream_path)
{
	if (!m_is_valid_ole || m_current_directory == nullptr)
		return nullptr;
	if (!m_child_directories_loaded)
	{
		if (!getCurrentDirectoryChilds())
			return nullptr;
		m_child_directories_loaded = true;
	}
	for (auto& m_child_directory : m_child_directories)
	{
		if (m_child_directory->m_name == stream_path)
		{
			if (m_child_directory->m_object_type != directory_entry::stream)
			{
				m_error = "Specified object is not a stream";
				return nullptr;
			}
			thread_safe_ole_stream_reader::stream stream;
			thread_safe_ole_stream_reader* ole_stream_reader = nullptr;
			stream.m_data_stream = nullptr;
			try
			{
				stream.m_data_stream = m_data_stream->clone();
				stream.m_size = m_child_directory->m_stream_size;
				if (stream.m_size < m_mini_stream_cut_off)
				{
					stream.m_sector_size = m_mini_sector_size;
					getStreamPositions(stream.m_file_positions, true, m_child_directory);
				}
				else
				{
					stream.m_sector_size = m_sector_size;
					getStreamPositions(stream.m_file_positions, false, m_child_directory);
				}
				ole_stream_reader = new thread_safe_ole_stream_reader(this, stream);
				if (!ole_stream_reader->isValid())
				{
					m_error = ole_stream_reader->getLastError();
					delete ole_stream_reader;
					return nullptr;
				}
				return ole_stream_reader;
			}
			catch (std::bad_alloc&)
			{
				delete stream.m_data_stream;
				delete ole_stream_reader;
				throw;
			}
		}
	}
	m_error = "Specified stream does not exist";
	return nullptr;
}

inline bool thread_safe_ole_storage::readDirectFromBuffer(unsigned char* buffer, int size, int offset)
{
	if (!m_data_stream->open())
	{
		m_error = "Cannot open file " + m_file_name;
		return false;
	}
	if (!m_data_stream->seek(offset, SEEK_SET))
	{
		m_error = "Cant seek to the selected position";
		m_data_stream->close();
		return false;
	}
	if (!m_data_stream->read(buffer, sizeof(unsigned char), size))
	{
		m_error = "Cant read from file";
		m_data_stream->close();
		return false;
	}
	m_data_stream->close();
	return true;
}

inline void thread_safe_ole_storage::streamDestroyed(wvWare::OLEStream* stream)
{
}

inline void thread_safe_ole_storage::getStreamPositions(std::vector<uint32_t>& stream_positions, bool mini_stream, const std::shared_ptr<directory_entry>& dir_entry)
{
	stream_positions.clear();
	size_t mini_sectors_count = m_number_of_mini_fat_sectors * m_sector_size / 4;
	size_t sectors_count = m_number_of_fat_sectors * m_sector_size / 4;
	if (mini_stream)
	{
		uint32_t current_mini_sector_index = 0;
		uint32_t mini_sectors_in_sector = m_sector_size / m_mini_sector_size;
		uint32_t mini_sector_location = m_directories[0]->m_start_sector_location;
		uint32_t mini_sector_position = dir_entry->m_start_sector_location;
		while (mini_sector_position != 0xFFFFFFFE)
		{
			uint32_t sector_index = mini_sector_position / mini_sectors_in_sector;
			if (sector_index != current_mini_sector_index)
			{
				current_mini_sector_index = sector_index;
				mini_sector_location = m_directories[0]->m_start_sector_location;
				while (sector_index > 0)
				{
					if (mini_sector_location >= sectors_count)
					{
						stream_positions.clear();
						return;
					}
					mini_sector_location = m_sectors_chain[mini_sector_location];
					--sector_index;
				}
			}
			uint32_t mini_sector_offset = mini_sector_position - current_mini_sector_index * mini_sectors_in_sector;
			uint32_t position = (1 + mini_sector_location) * m_sector_size + mini_sector_offset * m_mini_sector_size;
			stream_positions.push_back(position);
			if (mini_sector_position >= mini_sectors_count)
			{
				stream_positions.clear();
				return;
			}
			mini_sector_position = m_mini_sectors_chain[mini_sector_position];
		}
	}
	else
	{
		uint32_t sector_location = dir_entry->m_start_sector_location;
		while (sector_location != 0xFFFFFFFE)
		{
			uint32_t position = (1 + sector_location) * m_sector_size;
			stream_positions.push_back(position);
			if (sector_location >= sectors_count)
			{
				stream_positions.clear();
				return;
			}
			sector_location = m_sectors_chain[sector_location];
		}
	}
}

inline bool thread_safe_ole_storage::getCurrentDirectoryChilds()
{
	for (auto& m_child_directory : m_child_directories)
		m_child_directory->m_added = false;
	if (m_current_directory->m_child == 0xFFFFFFFF)
		return true;
	if (m_current_directory->m_child >= m_directories.size())
	{
		m_error = "Index of directory entry is outside the vector";
		return false;
	}
	m_child_directories.push_back(m_directories[m_current_directory->m_child]);
	int index_start = 0;
	int index_end = 0;
	std::shared_ptr<directory_entry> current_dir;
	std::shared_ptr<directory_entry> added_dir;
	while (index_start != index_end + 1)
	{
		current_dir = m_child_directories[index_start];
		if (current_dir->m_left_sibling != 0xFFFFFFFF)
		{
			if (current_dir->m_left_sibling >= m_directories.size())
			{
				m_error = "Index of directory entry is outside the vector";
				return false;
			}
			added_dir = m_directories[current_dir->m_left_sibling];
			if (!added_dir->m_added)
			{
				m_child_directories.push_back(added_dir);
				++index_end;
				added_dir->m_added = true;
			}
		}
		if (current_dir->m_right_sibling != 0xFFFFFFFF)
		{
			if (current_dir->m_right_sibling >= m_directories.size())
			{
				m_error = "Index of directory entry is outside the vector";
				return false;
			}
			added_dir = m_directories[current_dir->m_right_sibling];
			if (!added_dir->m_added)
			{
				m_child_directories.push_back(added_dir);
				++index_end;
				added_dir->m_added = true;
			}
		}
		++index_start;
	}
	return true;
}

inline void thread_safe_ole_storage::getStoragesAndStreams()
{
	if (!m_is_valid_ole)
		return;
	size_t records_count = m_sector_size / 4;
	size_t directory_count_per_sector = m_sector_size / 128;
	std::shared_ptr<directory_entry> directory;
	uint32_t directory_location = m_first_sector_directory_location;
	while (directory_location != 0xFFFFFFFE)
	{
		if (!m_data_stream->seek((1 + directory_location) * m_sector_size, SEEK_SET))
		{
			m_error = "Position of sector is outside of the file!";
			m_is_valid_ole = false;
			return;
		}
		for (size_t i = 0; i < directory_count_per_sector; ++i)
		{
			try
			{
				directory.reset(new directory_entry());
				directory->m_added = false;
				uint16_t unichars[32];
				if (!m_data_stream->read(unichars, sizeof(uint16_t), 32))
				{
					m_error = "Error in reading directory name";
					m_is_valid_ole = false;
					return;
				}
				for (int j = 0; j < 32; ++j)
				{
					unsigned int ch = unichars[j];
					if (ch == 0)
						break;
					if (utf16_unichar_has_4_bytes(ch))
					{
						if (++j < 32)
							ch = (ch << 16) | unichars[j];
						else
							break;
					}
					directory->m_name += unichar_to_utf8(ch);
				}
				if (!skipBytes(2))
					return;
				uint8_t object_type;
				if (!m_data_stream->read(&object_type, sizeof(uint8_t), 1))
				{
					m_error = "Error in reading type of object";
					m_is_valid_ole = false;
					return;
				}
				if (object_type != 0x00 && object_type != 0x01 && object_type != 0x02 && object_type != 0x05)
				{
					m_error = "Invalid type of object";
					m_is_valid_ole = false;
					return;
				}
				directory->m_object_type = static_cast<directory_entry::object_type>(object_type);
				uint8_t color_flag;
				if (!m_data_stream->read(&color_flag, sizeof(uint8_t), 1))
				{
					m_error = "Error in reading color flag";
					m_is_valid_ole = false;
					return;
				}
				if (color_flag != 0x00 && color_flag != 0x01)
				{
					m_error = "Invalid color flag";
					m_is_valid_ole = false;
					return;
				}
				directory->m_color_flag = color_flag;
				if (!m_data_stream->read(&directory->m_left_sibling, sizeof(uint32_t), 1))
				{
					m_error = "Error in reading left sibling";
					m_is_valid_ole = false;
					return;
				}
				if (!m_data_stream->read(&directory->m_right_sibling, sizeof(uint32_t), 1))
				{
					m_error = "Error in reading directory right sibling";
					m_is_valid_ole = false;
					return;
				}
				if (!m_data_stream->read(&directory->m_child, sizeof(uint32_t), 1))
				{
					m_error = "Error in reading child";
					m_is_valid_ole = false;
					return;
				}
				if (!skipBytes(36))
					return;
				if (!m_data_stream->read(&directory->m_start_sector_location, sizeof(uint32_t), 1))
				{
					m_error = "Error in reading sector location";
					m_is_valid_ole = false;
					return;
				}
				if (!m_data_stream->read(&directory->m_stream_size, sizeof(uint64_t), 1))
				{
					m_error = "Error in reading sector size";
					m_is_valid_ole = false;
					return;
				}
				if (m_header_version == 0x03)
				{
					directory->m_stream_size = directory->m_stream_size & 0x00000000FFFFFFFF;
				}
				m_directories.push_back(directory);
			}
			catch (std::bad_alloc&)
			{
				throw;
			}
		}
		if (directory_location >= m_number_of_fat_sectors * records_count)
		{
			m_error = "Directory location is outside of the sector chain";
			m_is_valid_ole = false;
			return;
		}
		directory_location = m_sectors_chain[directory_location];
	}
	if (m_directories.empty())
	{
		m_is_valid_ole = false;
		m_error = "Root directory does not exist";
		return;
	}
	m_current_directory = m_directories[0];
	m_data_stream->close();
}

inline void thread_safe_ole_storage::getMiniFatSectorChain()
{
	if (!m_is_valid_ole)
		return;
	size_t records_count = m_sector_size / 4;
	m_mini_sectors_chain.resize(m_number_of_mini_fat_sectors * records_count);
	size_t mini_sector_location = m_first_mini_fat_sector_location;
	for (size_t i = 0; i < m_number_of_mini_fat_sectors; ++i)
	{
		if (!m_data_stream->seek((1 + mini_sector_location) * m_sector_size, SEEK_SET))
		{
			m_error = "Position of sector is outside of the file!";
			m_is_valid_ole = false;
			return;
		}
		if (!m_data_stream->read(&m_mini_sectors_chain[i * records_count], sizeof(uint32_t), records_count))
		{
			m_error = "Error in reading sector chain";
			m_is_valid_ole = false;
			return;
		}
		if (mini_sector_location >= m_number_of_fat_sectors * records_count)
		{
			m_error = "Mini sector location is outside of the sector chain";
			m_is_valid_ole = false;
			return;
		}
		mini_sector_location = m_sectors_chain[mini_sector_location];
		if (mini_sector_location == 0xFFFFFFFE)
			break;
	}
}

inline void thread_safe_ole_storage::getFatSectorChain()
{
	if (!m_is_valid_ole)
		return;
	size_t records_count = m_sector_size / 4;
	m_sectors_chain.resize(m_number_of_fat_sectors * records_count);
	for (size_t i = 0; i < m_number_of_fat_sectors; ++i)
	{
		if (!m_data_stream->seek((1 + m_fat_sectors_chain[i]) * m_sector_size, SEEK_SET))
		{
			m_error = "Position of sector is outside of the file!";
			m_is_valid_ole = false;
			return;
		}
		if (!m_data_stream->read(&m_sectors_chain[i * records_count], sizeof(uint32_t), records_count))
		{
			m_error = "Error in reading sector chain";
			m_is_valid_ole = false;
			return;
		}
	}
}

inline void thread_safe_ole_storage::getFatArraySectorChain()
{
	if (!m_is_valid_ole)
		return;
	uint32_t records_count = m_sector_size / 4 - 1;
	m_fat_sectors_chain.resize(m_number_of_fat_sectors);
	uint32_t remaining_fat_sector_chain_count = m_number_of_fat_sectors;
	for (int i = 0; i < 109; ++i)
	{
		if (remaining_fat_sector_chain_count == 0)
			break;
		if (!getUint32(m_fat_sectors_chain[i]))
			return;
		--remaining_fat_sector_chain_count;
	}
	uint32_t difat_sector_location = m_first_difat_sector_location;
	for (uint32_t i = 0; i < m_number_of_difat_sectors; ++i)
	{
		if (!m_data_stream->seek((1 + difat_sector_location) * m_sector_size, SEEK_SET))
		{
			m_error = "Position of sector is outside of the file!";
			m_is_valid_ole = false;
			return;
		}
		for (uint32_t j = 0; j < records_count; ++j)
		{
			if (remaining_fat_sector_chain_count == 0)
				break;
			if (!getUint32(m_fat_sectors_chain[109 + i * records_count + j]))
				return;
			--remaining_fat_sector_chain_count;
		}
		if (!getUint32(difat_sector_location))
			return;
		if (difat_sector_location == 0xFFFFFFFE)
			break;
	}
}

inline bool thread_safe_ole_storage::skipBytes(int bytes_to_skip)
{
	if (!m_data_stream->seek(bytes_to_skip, SEEK_CUR))
	{
		m_is_valid_ole = false;
		m_error = "Cant seek";
		return false;
	}
	return true;
}

inline bool thread_safe_ole_storage::getUint16(uint16_t& data)
{
	if (!m_data_stream->read(&data, sizeof(uint16_t), 1))
	{
		m_is_valid_ole = false;
		m_error = "Error in reading 16-bit number";
		return false;
	}
	return true;
}

inline bool thread_safe_ole_storage::getUint32(uint32_t& data)
{
	if (!m_data_stream->read(&data, sizeof(uint32_t), 1))
	{
		m_is_valid_ole = false;
		m_error = "Error in reading 32-bit number";
		return false;
	}
	return true;
}

inline void thread_safe_ole_storage::parseHeader()
{
	uint8_t ole_header[] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
	uint8_t readed_ole_header[8];
	if (!m_is_valid_ole)
		return;
	if (!m_data_stream->read(readed_ole_header, sizeof(uint8_t), 8) ||
			memcmp(readed_ole_header, ole_header, 8) != 0)
	{
		m_is_valid_ole = false;
		m_error = "Header is invalid: no OLE signature";
		return;
	}
	if (!skipBytes(18))
		return;
	if (!getUint16(m_header_version))
		return;
	if (!getUint16(m_byte_order))
		return;
	if (!getUint16(m_sector_size))
		return;
	m_sector_size = static_cast<uint16_t>(pow(2, m_sector_size));
	if (!getUint16(m_mini_sector_size))
		return;
	m_mini_sector_size = static_cast<uint16_t>(pow(2, m_mini_sector_size));
	if (!skipBytes(6))
		return;
	if (!getUint32(m_number_of_directories))
		return;
	if (!getUint32(m_number_of_fat_sectors))
		return;
	if (!getUint32(m_first_sector_directory_location))
		return;
	if (!skipBytes(4))
		return;
	if (!getUint32(m_mini_stream_cut_off))
		return;
	if (!getUint32(m_first_mini_fat_sector_location))
		return;
	if (!getUint32(m_number_of_mini_fat_sectors))
		return;
	if (!getUint32(m_first_difat_sector_location))
		return;
	if (!getUint32(m_number_of_difat_sectors))
		return;
}

} // namespace docwire

#endif // DOCWIRE_THREAD_SAFE_OLE_STORAGE_H
