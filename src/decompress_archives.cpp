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

#include "decompress_archives.h"

#include <archive.h>
#include <archive_entry.h>
#include "exception.h"
#include <filesystem>
#include <fstream>
#include "log.h"
#include "parser.h"

namespace docwire
{

DecompressArchives::DecompressArchives()
{
}

DecompressArchives::DecompressArchives(const DecompressArchives &other)
{
}

DecompressArchives::~DecompressArchives()
{
}

class ArchiveReader
{
public:
	class EntryStreamBuf : public std::streambuf
	{
	public:
		EntryStreamBuf(archive* archive)
			: m_archive(archive)
		{
		}

		int_type underflow()
		{
			docwire_log(debug) << "Archive reader buffer underflow";
			la_ssize_t bytes_read = archive_read_data(m_archive, m_buffer, m_buf_size);
			docwire_log(debug) << bytes_read << " bytes read";
			if (bytes_read < 0)
				throw Exception(archive_error_string(m_archive));
			if (bytes_read == 0)
				return traits_type::eof();
			setg(m_buffer, m_buffer, m_buffer + bytes_read);
			return traits_type::to_int_type(*gptr());
		}

	private:
		archive* m_archive;
		static constexpr size_t m_buf_size = 16384;
		char m_buffer[m_buf_size];
	};

	class EntryIStream : public std::istream
	{
	public:
		EntryIStream(archive* archive)
			: std::istream(new EntryStreamBuf(archive)) {}

		~EntryIStream() { delete rdbuf(); }
	};

	class Entry
	{
	public:
		Entry(archive* archive, archive_entry* entry)
			: m_archive(archive), m_entry(entry) {}

		std::string get_name() { return archive_entry_pathname(m_entry); }

		bool is_dir() { return (archive_entry_mode(m_entry) & AE_IFDIR); }

		EntryIStream create_stream() { return EntryIStream(m_archive); }

		operator bool() { return m_entry != nullptr; }

		bool operator!= (const Entry& r) { return m_entry != r.m_entry; };

	private:
		archive* m_archive;
		archive_entry* m_entry;
	};

	struct Iterator
	{
		ArchiveReader* m_reader;
		Entry m_entry;
		Iterator(ArchiveReader* reader, Entry entry)
			: m_reader(reader),
				m_entry(entry)
		{
		}
		Iterator& operator++() { m_entry = m_reader->get_next_entry(); return *this; }
		bool operator!= (const Iterator& r) { return m_entry != r.m_entry; };
		const Entry& operator*() const { return m_entry; }
	};

	ArchiveReader(std::istream& stream)
		: data(stream)
	{
		m_archive = archive_read_new();
		archive_read_support_filter_all(m_archive);
		archive_read_support_format_all(m_archive);
		int r = archive_read_open(m_archive, &data, nullptr, archive_read_callback, archive_close_callback);
		if (r != ARCHIVE_OK)
			throw Exception(archive_error_string(m_archive));
	}

	~ArchiveReader()
	{
		int r = archive_read_free(m_archive);
		if (r != ARCHIVE_OK)
			docwire_log(error) << "archive_read_free() error: " << archive_error_string(m_archive);
	}

	archive* get_archive()
	{
		return m_archive;
	}

	Entry get_next_entry()
	{
		archive_entry* entry;
		int r = archive_read_next_header(m_archive, &entry);
		if (r == ARCHIVE_EOF)
		{
			docwire_log(debug) << "End of archive";
			return Entry(m_archive, nullptr);
		}
		if (r != ARCHIVE_OK)
		{
			docwire_log(error) << "archive_read_next_header() error: " << archive_error_string(m_archive);
			throw Exception(archive_error_string(m_archive));
		}
		return Entry(m_archive, entry);
	}

	Iterator begin()
	{
		return Iterator(this, get_next_entry());
	}

	Iterator end()
	{
		return Iterator(this, Entry(m_archive, nullptr));
	}

private:
	struct CallbackClientData
	{
		CallbackClientData(std::istream& stream)
			: m_stream(stream)
		{
		}
		std::istream& m_stream;
		static constexpr size_t m_buf_size = 16384;
		char m_buffer[m_buf_size];
	};

	archive* m_archive;
	CallbackClientData data;

	static la_ssize_t archive_read_callback(archive* archive, void* client_data, const void** buf)
	{
		docwire_log(debug) << "archive_read_callback()";
		CallbackClientData* data = (CallbackClientData*)client_data;
		*buf = data->m_buffer;
		if (data->m_stream.read(data->m_buffer, data->m_buf_size))
			return data->m_buf_size;
		else
		{
			if (!data->m_stream.eof())
			{
				archive_set_error(archive, EIO, "Stream reading error");
				return -1;
			}
			return data->m_stream.gcount();
		}
	}

	static int archive_close_callback(archive* archive, void* client_data)
	{
		return ARCHIVE_OK;
	}
};

void
DecompressArchives::process(Info &info) const
{
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "TAG_FILE received";
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	std::optional<std::string> name = info.getAttributeValue<std::string>("name");
	docwire_log_vars(path, name);
	if(!path && !stream)
		throw Exception("No path or stream in TAG_FILE");
	auto is_supported = [](const std::string& fn)
	{
		std::set<std::string> supported_extensions { ".zip", ".tar", ".rar", ".gz", ".bz2", ".xz" };
		return supported_extensions.count(std::filesystem::path(fn).extension().string()) > 0;
	};
	if ((path && !is_supported(*path)) || (name && !is_supported(*name)))
	{
		docwire_log(debug) << "Filename extension shows it is not an supported archive, skipping.";
		emit(info);
		return;
	}
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	try
	{
		docwire_log(debug) << "Decompressing archive";
		ArchiveReader reader(*in_stream);
		for (ArchiveReader::Entry entry: reader)
		{
			std::string entry_name = entry.get_name();
			docwire_log(debug) << "Processing compressed file " << entry_name;
			if (entry.is_dir())
			{
				docwire_log(debug) << "Skipping directory entry";
				continue;
			}
			ArchiveReader::EntryIStream entry_stream = entry.create_stream();
			Info info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&entry_stream}, {"name", entry_name}});
			process(info);
			docwire_log(debug) << "End of processing compressed file " << entry_name;
		}
		docwire_log(debug) << "Archive decompressed successfully";
	}
	catch (Exception& e)
	{
		docwire_log(error) << e.what();
		in_stream->clear();
		in_stream->seekg(std::ios::beg);
		emit(info);
	}
	if (path)
		delete in_stream;
}

DecompressArchives* DecompressArchives::clone() const
{
	return new DecompressArchives(*this);
}

} // namespace docwire
