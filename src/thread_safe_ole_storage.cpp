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

#include "thread_safe_ole_storage.h"

#include "data_stream.h"
#include <cmath>
#include "misc.h"
#include <new>
#include <memory>
#include <cstdio>
#include <cstring>
#include "thread_safe_ole_stream_reader.h"

struct ThreadSafeOLEStorage::Implementation
{
	bool m_is_valid_ole;
	std::string m_error;
	std::string m_file_name;
	DataStream* m_data_stream;
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

	struct DirectoryEntry
	{
		std::string m_name;
		enum ObjectType
		{
			unknown_unallocated = 0x01,
			storage = 0x01,
			stream = 0x02,
			root_storage = 0x05
		};
		ObjectType m_object_type;
		uint8_t m_color_flag{0};
		uint32_t m_left_sibling{0};
		uint32_t m_right_sibling{0};
		uint32_t m_child{0};
		uint32_t m_start_sector_location{0};
		uint64_t m_stream_size{0};
		bool m_added{false};
	};
	std::shared_ptr<DirectoryEntry> m_current_directory;
	std::vector<std::shared_ptr<DirectoryEntry>> m_directories;
	std::vector<std::shared_ptr<DirectoryEntry>> m_child_directories;
	std::vector<std::shared_ptr<DirectoryEntry>> m_inside_directories;
	bool m_child_directories_loaded;

	explicit Implementation(const std::string &file_name)
	{
		m_file_name = file_name;
		m_is_valid_ole = true;
		m_data_stream = nullptr;
		m_data_stream = new FileStream(file_name);
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

	Implementation(const char* buffer, size_t size)
	{
		m_file_name = "Memory buffer";
		m_is_valid_ole = true;
		m_data_stream = nullptr;
		m_data_stream = new BufferStream(buffer, size);
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

	~Implementation()
	{
		delete m_data_stream;
	}

	void getStreamPositions(std::vector<uint32_t>& stream_positions, bool mini_stream, const std::shared_ptr<DirectoryEntry>& dir_entry)
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

	bool getCurrentDirectoryChilds()
	{
		for (auto & m_child_directory : m_child_directories)
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
		std::shared_ptr<Implementation::DirectoryEntry> current_dir;
		std::shared_ptr<Implementation::DirectoryEntry> added_dir;
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

	void getStoragesAndStreams()
	{
		if (!m_is_valid_ole)
			return;
		size_t records_count = m_sector_size / 4;
		size_t directory_count_per_sector = m_sector_size / 128;
		std::shared_ptr<DirectoryEntry> directory;
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
					directory.reset(new DirectoryEntry());
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
					directory->m_object_type = (DirectoryEntry::ObjectType)object_type;
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
				catch (std::bad_alloc& ba)
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

	void getMiniFatSectorChain()
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

	void getFatSectorChain()
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

	void getFatArraySectorChain()
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

	bool skipBytes(int bytes_to_skip)
	{
		if (!m_data_stream->seek(bytes_to_skip, SEEK_CUR))
		{
			m_is_valid_ole = false;
			m_error = "Cant seek";
			return false;
		}
		return  true;
	}

	bool getUint16(uint16_t& data)
	{
		if (!m_data_stream->read(&data, sizeof(uint16_t), 1))
		{
			m_is_valid_ole = false;
			m_error = "Error in reading 16-bit number";
			return false;
		}
		return true;
	}

	bool getUint32(uint32_t& data)
	{
		if (!m_data_stream->read(&data, sizeof(uint32_t), 1))
		{
			m_is_valid_ole = false;
			m_error = "Error in reading 32-bit number";
			return false;
		}
		return true;
	}

	void parseHeader()
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
		m_sector_size = (uint16_t)pow(2, m_sector_size);
		if (!getUint16(m_mini_sector_size))
			return;
		m_mini_sector_size = (uint16_t)pow(2, m_mini_sector_size);
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
};

ThreadSafeOLEStorage::ThreadSafeOLEStorage(const std::string &file_name)
{
	impl = nullptr;
	try
	{
		impl = new Implementation(file_name);
	}
	catch (std::bad_alloc& ba)
	{
    delete impl;
		throw;
	}
}

ThreadSafeOLEStorage::ThreadSafeOLEStorage(const char *buffer, size_t len)
{
	impl = nullptr;
	try
	{
		impl = new Implementation(buffer, len);
	}
	catch (std::bad_alloc& ba)
	{
			delete impl;

		throw;
	}
}

ThreadSafeOLEStorage::~ThreadSafeOLEStorage()
{


		delete impl;

}

bool ThreadSafeOLEStorage::open(Mode mode)
{
	if (mode == ReadOnly)	//opening in constructor
		return true;
	return false;			//only read mode is supported now
}

bool ThreadSafeOLEStorage::isValid() const
{
	return impl->m_is_valid_ole;
}

void ThreadSafeOLEStorage::close()
{
	if (impl != nullptr)
	{
		delete impl;
		impl = nullptr;
	}
}

std::string ThreadSafeOLEStorage::getLastError()
{
	return impl->m_error;
}

std::string ThreadSafeOLEStorage::name() const
{
	return impl->m_file_name;
}

bool ThreadSafeOLEStorage::getStreamsAndStoragesList(std::vector<std::string>& components)
{
	components.clear();
	if (impl == nullptr || !impl->m_is_valid_ole || impl->m_current_directory == nullptr)
		return false;
	if (!impl->m_child_directories_loaded)
	{
		if (!impl->getCurrentDirectoryChilds())
			return false;
		impl->m_child_directories_loaded = true;
	}
	for (auto & m_child_directory : impl->m_child_directories)
	{
		components.push_back(m_child_directory->m_name);
	}
	return true;
}

bool ThreadSafeOLEStorage::enterDirectory(const std::string& directory_path)
{
	if (impl == nullptr || !impl->m_is_valid_ole || impl->m_current_directory == nullptr)
		return false;
	if (!impl->m_child_directories_loaded)
	{
		if (!impl->getCurrentDirectoryChilds())
			return false;
		impl->m_child_directories_loaded = true;
	}
	for (size_t i = 0; i < impl->m_child_directories.size(); ++i)
	{
		if (impl->m_child_directories[i]->m_name == directory_path)
		{
			if (impl->m_child_directories[i]->m_object_type != Implementation::DirectoryEntry::storage)
			{
				impl->m_error = "Specified object is not directory";
				return false;
			}
			impl->m_inside_directories.push_back(impl->m_current_directory);
			impl->m_current_directory = impl->m_child_directories[i];
			impl->m_child_directories_loaded = false;
			for (auto & m_child_directory : impl->m_child_directories)
				m_child_directory->m_added = false;
			impl->m_child_directories.clear();
			return true;
		}
	}
	impl->m_error = "Specified directory does not exist";
	return false;
}

bool ThreadSafeOLEStorage::leaveDirectory()
{
	if (impl == nullptr || !impl->m_is_valid_ole || impl->m_current_directory == nullptr)
		return false;
	if (impl->m_inside_directories.empty())
	{
		impl->m_error = "Already in root directory";
		return false;
	}
	impl->m_current_directory = impl->m_inside_directories.back();
	impl->m_inside_directories.pop_back();
	impl->m_child_directories_loaded = false;
	for (auto & m_child_directory : impl->m_child_directories)
		m_child_directory->m_added = false;
	impl->m_child_directories.clear();
	return true;
}

AbstractOLEStreamReader *ThreadSafeOLEStorage::createStreamReader(const std::string& stream_path)
{
	if (impl == nullptr || !impl->m_is_valid_ole || impl->m_current_directory == nullptr) {
        return nullptr;
    }
	if (!impl->m_child_directories_loaded)
	{
		if (!impl->getCurrentDirectoryChilds()) {
            return nullptr;
        }
		impl->m_child_directories_loaded = true;
	}
	for (auto & m_child_directory : impl->m_child_directories)
	{
		if (m_child_directory->m_name == stream_path)
		{
			if (m_child_directory->m_object_type != Implementation::DirectoryEntry::stream)
			{
				impl->m_error = "Specified object is not a stream";
				return nullptr;
			}
			ThreadSafeOLEStreamReader::Stream stream;
			ThreadSafeOLEStreamReader* ole_stream_reader = nullptr;
			stream.m_data_stream = nullptr;
			try
			{
				stream.m_data_stream = impl->m_data_stream->clone();
				stream.m_size = m_child_directory->m_stream_size;
				if (stream.m_size < impl->m_mini_stream_cut_off)
				{
					stream.m_sector_size = impl->m_mini_sector_size;
					impl->getStreamPositions(stream.m_file_positions, true, m_child_directory);
				}
				else
				{
					stream.m_sector_size = impl->m_sector_size;
					impl->getStreamPositions(stream.m_file_positions, false, m_child_directory);
				}
				ole_stream_reader = new ThreadSafeOLEStreamReader(this, stream);
				if (!ole_stream_reader->isValid())
				{
					impl->m_error = ole_stream_reader->getLastError();
					delete ole_stream_reader;
					return nullptr;
				}
				return ole_stream_reader;
			}
			catch (std::bad_alloc& ba)
			{

					delete stream.m_data_stream;

					delete ole_stream_reader;
				throw;
			}
		}
	}
	impl->m_error = "Specified stream does not exist";
	return nullptr;
}

bool ThreadSafeOLEStorage::readDirectFromBuffer(unsigned char* buffer, int size, int offset)
{
	if (!impl->m_data_stream->open())
	{
		impl->m_error = "Cannot open file " + impl->m_file_name;
		return false;
	}
	if (!impl->m_data_stream->seek(offset, SEEK_SET))
	{
		impl->m_error = "Cant seek to the selected position";
		impl->m_data_stream->close();
		return false;
	}
	if (!impl->m_data_stream->read(buffer, sizeof(unsigned char), size))
	{
		impl->m_error = "Cant read from file";
		impl->m_data_stream->close();
		return false;
	}
	impl->m_data_stream->close();
	return true;
}

void ThreadSafeOLEStorage::streamDestroyed(OLEStream* stream)
{
	//nothing to do. Stream is already self-sufficient and storage does not care about it
}
