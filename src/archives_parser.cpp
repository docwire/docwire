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

#include "archives_parser.h"

#include <archive.h>
#include <archive_entry.h>
#include "data_source.h"
#include "error_tags.h"
#include <filesystem>
#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "make_error.h"
#include "nested_exception.h"
#include "serialization_message.h" // IWYU pragma: keep
#include "throw_if.h"
#include <vector>
#include "message_counters.h"

namespace docwire
{

namespace
{

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/zip-compressed"},
	mime_type{"application/x-zip-compressed"},
    mime_type{"application/zip"},	
    mime_type{"application/x-tar"},
	mime_type{"application/x-rar-compressed"},
    mime_type{"application/vnd.rar"},
    mime_type{"application/gzip"},
    mime_type{"application/x-gzip"},
    mime_type{"application/x-bzip2"},
    mime_type{"application/x-xz"}
};

} // anonymous namespace

template<typename auditor_type>
class archive_reader : private audit::with_auditor<auditor_type>
{
public:
	using audit::with_auditor<auditor_type>::auditor;

	class entry_streambuf : public std::streambuf
	{
	public:
		entry_streambuf(archive* archive, audit::auditor_ref<auditor_type> auditor)
			: m_archive(archive), m_auditor(auditor)
		{
		}

		int_type underflow()
		{
			audit::scope audit_scope{m_auditor, audit::no_context};
			la_ssize_t bytes_read = archive_read_data(m_archive, m_buffer, m_buf_size);
			audit::entry(m_auditor, [&]{ return "bytes_read"_v = bytes_read; });
			throw_if (bytes_read < 0, "archive_read_data() failed", archive_error_string(m_archive));
			if (bytes_read == 0)
				return traits_type::eof();
			setg(m_buffer, m_buffer, m_buffer + bytes_read);
			return traits_type::to_int_type(*gptr());
		}

	private:
		archive* m_archive;
		audit::auditor_ref<auditor_type> m_auditor;
		static constexpr size_t m_buf_size = 16384;
		char m_buffer[m_buf_size];
	};

	class entry_istream : public std::istream
	{
	public:
		entry_istream(archive* archive, audit::auditor_ref<auditor_type> auditor)
			: std::istream(new entry_streambuf(archive, auditor)) {}

		~entry_istream() { delete rdbuf(); }
	};

	class entry
	{
	public:
		entry(archive* archive, archive_entry* entry, audit::auditor_ref<auditor_type> auditor)
			: m_archive(archive), m_entry(entry), m_auditor(auditor) {}

		std::string get_name() { return archive_entry_pathname(m_entry); }

		bool is_dir() { return (archive_entry_mode(m_entry) & AE_IFDIR); }

		std::unique_ptr<entry_istream> create_stream() { return std::make_unique<entry_istream>(m_archive, m_auditor); }

		operator bool() { return m_entry != nullptr; }

		bool operator!= (const entry& r) { return m_entry != r.m_entry; };

	private:
		archive* m_archive;
		archive_entry* m_entry;
		audit::auditor_ref<auditor_type> m_auditor;
	};

	struct iterator
	{
		archive_reader* m_reader;
		entry m_entry;
		iterator(archive_reader* reader, entry entry)
			: m_reader(reader),
				m_entry(entry)
		{
		}
		iterator& operator++() { m_entry = m_reader->get_next_entry(); return *this; }
		bool operator!= (const iterator& r) { return m_entry != r.m_entry; };
		const entry& operator*() const { return m_entry; }
	};

	archive_reader(std::istream& stream, const std::function<void(std::exception_ptr)>& non_fatal_error_handler,
	               audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>{auditor},
		  data(stream, auditor),
		  m_non_fatal_error_handler(non_fatal_error_handler)
	{
		m_archive = archive_read_new();
		archive_read_support_filter_all(m_archive);
		archive_read_support_format_all(m_archive);
		int r = archive_read_open(m_archive, &data, nullptr, archive_read_callback, archive_close_callback);
		throw_if (r != ARCHIVE_OK, "archive_read_open() failed", archive_error_string(m_archive));
	}

	~archive_reader()
	{
		int r = archive_read_free(m_archive);
		if (r != ARCHIVE_OK)
			m_non_fatal_error_handler(make_error_ptr("archive_read_free() error", archive_error_string(m_archive)));
	}

	archive* get_archive()
	{
		return m_archive;
	}

	entry get_next_entry()
	{
		archive_entry* entry;
		int r = archive_read_next_header(m_archive, &entry);
		if (r == ARCHIVE_EOF)
		{
			audit::entry(auditor(), [&]{ return "message"_v = std::string{"End of archive"}; });
			return archive_reader::entry(m_archive, nullptr, auditor());
		}
		throw_if (r != ARCHIVE_OK, "archive_read_next_header() failed", archive_error_string(m_archive));
		return archive_reader::entry(m_archive, entry, auditor());
	}

	iterator begin()
	{
		return iterator(this, get_next_entry());
	}

	iterator end()
	{
		return iterator(this, entry(m_archive, nullptr, auditor()));
	}

private:
	struct callback_client_data
	{
		callback_client_data(std::istream& stream, audit::auditor_ref<auditor_type> auditor)
			: m_stream(stream), m_auditor(auditor)
		{
		}
		std::istream& m_stream;
		audit::auditor_ref<auditor_type> m_auditor;
		static constexpr size_t m_buf_size = 16384;
		char m_buffer[m_buf_size];
	};

	archive* m_archive;
	callback_client_data data;
	std::function<void(std::exception_ptr)> m_non_fatal_error_handler;

	static la_ssize_t archive_read_callback(archive* archive, void* client_data, const void** buf)
	{
		callback_client_data* data = (callback_client_data*)client_data;
		audit::scope audit_scope{data->m_auditor, audit::no_context};
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

template<typename auditor_type>
continuation archives_parser<auditor_type>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};

	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	
	data_source& data = msg->get<data_source>();
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_message(std::move(msg));

	std::shared_ptr<std::istream> in_stream = data.istream();

	try
	{
		archive_reader<auditor_type> reader{
			*in_stream,
			[&emit_message](std::exception_ptr e) { emit_message(std::move(e)); },
			auditor()
		};

		message_counters counters;
		auto counting_callbacks = make_counted_message_callbacks(emit_message, counters);
		for (auto entry: reader)
		{
			std::string entry_name = entry.get_name();
			audit::scope entry_audit_scope{auditor(), [&]{ return "entry_name"_v = entry_name; }};

			if (entry.is_dir())
			{
				audit::entry(auditor(), audit::no_context);
				continue;
			}

			try
			{
				data_source entry_data_source{
					unseekable_stream_ptr{entry.create_stream()}, 
					file_extension{std::filesystem::path{entry_name}}
				};
				if (counting_callbacks.back(std::move(entry_data_source)) == continuation::stop)
					return continuation::stop;
			}
			catch (const std::exception&)
			{
				emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process archive entry", entry_name)));
			}
		}
		if (counters.all_failed())
			throw make_error("No entries were successfully processed", errors::uninterpretable_data{});
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error processing archive"));
	}
	return continuation::proceed;
}

template class DOCWIRE_ARCHIVES_EXPORT archives_parser<docwire::audit::null_auditor>;
template class DOCWIRE_ARCHIVES_EXPORT archives_parser<docwire::audit::json_stream_auditor>;

} // namespace docwire
