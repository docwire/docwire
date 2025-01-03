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
#include <filesystem>
#include "log.h"
#include "parser.h"
#include <set>
#include "throw_if.h"

namespace docwire
{

DecompressArchives::DecompressArchives()
{
}

DecompressArchives::DecompressArchives(const DecompressArchives &other)
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
			throw_if (bytes_read < 0, "archive_read_data() failed", archive_error_string(m_archive));
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

		std::unique_ptr<EntryIStream> create_stream() { return std::make_unique<EntryIStream>(m_archive); }

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

	ArchiveReader(std::istream& stream, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
		: data(stream), m_non_fatal_error_handler(non_fatal_error_handler)
	{
		m_archive = archive_read_new();
		archive_read_support_filter_all(m_archive);
		archive_read_support_format_all(m_archive);
		int r = archive_read_open(m_archive, &data, nullptr, archive_read_callback, archive_close_callback);
		throw_if (r != ARCHIVE_OK, "archive_read_open() failed", archive_error_string(m_archive));
	}

	~ArchiveReader()
	{
		int r = archive_read_free(m_archive);
		if (r != ARCHIVE_OK)
			m_non_fatal_error_handler(make_error_ptr("archive_read_free() error", archive_error_string(m_archive)));
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
		throw_if (r != ARCHIVE_OK, "archive_read_next_header() failed", archive_error_string(m_archive));
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
	std::function<void(std::exception_ptr)> m_non_fatal_error_handler;

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
DecompressArchives::process(Info& info)
{
	if (!std::holds_alternative<data_source>(info.tag))
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "data_source received";
	const data_source& data = std::get<data_source>(info.tag);
	auto is_supported = [](const file_extension& fe)
	{
		static const std::set<file_extension> supported_extensions { file_extension{".zip"}, file_extension{".tar"}, file_extension{".rar"}, file_extension{".gz"}, file_extension{".bz2"}, file_extension{".xz"} };
		return supported_extensions.count(fe) > 0;
	};
	if (!data.file_extension() || !is_supported(*data.file_extension()))
	{
		docwire_log(debug) << "Filename extension shows it is not an supported archive, skipping.";
		emit(info);
		return;
	}
	std::shared_ptr<std::istream> in_stream = data.istream();
	try
	{
		docwire_log(debug) << "Decompressing archive";
		ArchiveReader reader(*in_stream, [this](std::exception_ptr e) { Info info{e}; emit(info); });
		for (ArchiveReader::Entry entry: reader)
		{
			std::string entry_name = entry.get_name();
			docwire_log(debug) << "Processing compressed file " << entry_name;
			if (entry.is_dir())
			{
				docwire_log(debug) << "Skipping directory entry";
				continue;
			}
			Info info(data_source{unseekable_stream_ptr{entry.create_stream()}, file_extension{std::filesystem::path{entry_name}}});
			process(info);
			docwire_log(debug) << "End of processing compressed file " << entry_name;
		}
		docwire_log(debug) << "Archive decompressed successfully";
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error processing archive"));
	}
}

} // namespace docwire
