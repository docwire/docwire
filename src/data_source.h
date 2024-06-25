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

#ifndef DOCWIRE_DATA_SOURCE_H
#define DOCWIRE_DATA_SOURCE_H

#include "exception.h"
#include "file_extension.h"
#include <filesystem>
#include <fstream>
#include "log.h"
#include "memory_buffer.h"
#include "memorystream.h"
#include <optional>
#include "unique_identifier.h"
#include <variant>
#include <vector>

namespace docwire
{

struct seekable_stream_ptr
{
  std::shared_ptr<std::istream> v;
};

struct unseekable_stream_ptr
{
  std::shared_ptr<std::istream> v;
};

struct length_limit
{
	size_t v;
};

template <typename T>
concept data_source_compatible_type =
	std::is_same_v<T, std::filesystem::path> ||
	std::is_same_v<T, std::vector<std::byte>> ||
	std::is_same_v<T, std::span<const std::byte>> ||
	std::is_same_v<T, std::string> ||
	std::is_same_v<T, std::string_view> ||
	std::is_same_v<T, seekable_stream_ptr> ||
	std::is_same_v<T, unseekable_stream_ptr>;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/**
	The class below represents a binary data source for data processing.
	It can be initialized with a file path, memory buffer, input stream or other data source.
	All popular C++ data sources are supported.
	Document parsers and 3rdparty libraries needs to have access to the data in preferred way
	like memory buffer or file path or stream or range, because of their implementation and it cannot be changed.
	Sometimes one method is faster than other, and parser needs to know about state of data source to decide.
	Converting data from one storage form to other should be possible in all combinations but performed only
	as required (lazy) and cached inside the class, for example file should be read to memory only once.
	Performance is very important, for example we should not duplicate memory buffer that is passed to class.
**/
class data_source
{
	public:
		
		template <data_source_compatible_type T>
		explicit data_source(const T& source)
			: m_source{source}
		{}

		template <data_source_compatible_type T>
		explicit data_source(T&& source)
			: m_source{std::move(source)}
		{}

		template <data_source_compatible_type T>
		explicit data_source(const T& source, file_extension file_extension)
			: m_source{source}, m_file_extension{file_extension}
		{}

		template <data_source_compatible_type T>
		explicit data_source(T&& source, file_extension file_extension)
			: m_source{std::move(source)}, m_file_extension{file_extension}
		{}

		std::span<const std::byte> span() const
		{
			return std::visit(
    			overloaded {
					[this](const std::vector<std::byte>& source)
					{
						return std::span{source.data(), source.size()};
					},
        			[this](const std::span<const std::byte>& source)
					{
						return source;
					},
					[this](const std::string& source)
					{
						return std::span{reinterpret_cast<const std::byte*>(source.data()), source.size()};
					},
					[this](const std::string_view& source)
					{
						return std::span{reinterpret_cast<const std::byte*>(source.data()), source.size()};
					},
					[this](auto source)
					{
						fill_memory_cache();
						return static_cast<std::span<const std::byte>>(m_memory_cache->span());
					}
				}, m_source);
		}

		std::string string(std::optional<length_limit> limit = std::nullopt) const
		{
			return std::visit(
    			overloaded {
					[this, limit](const std::vector<std::byte>& source)
					{
						if (limit)
							return std::string{reinterpret_cast<const char*>(source.data()), std::min(source.size(), limit->v)};
						else
							return std::string{reinterpret_cast<const char*>(source.data()), source.size()};
					},
					[this, limit](const std::span<const std::byte>& source)
					{
						if (limit)
							return std::string{reinterpret_cast<const char*>(source.data()), std::min(source.size(), limit->v)};
						else
							return std::string{reinterpret_cast<const char*>(source.data()), source.size()};
					},
					[this, limit](const std::string& source)
					{
						if (limit)
							return source.substr(0, limit->v);
						else
							return source;
					},
					[this, limit](const std::string_view& source)
					{
						if (limit)
							return std::string{source.substr(0, limit->v)};
						else
							return std::string{source};
					},
					[this, limit](auto source)
					{
						fill_memory_cache();
						if (limit)
							return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), std::min(m_memory_cache->size(), limit->v)};
						else
							return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), m_memory_cache->size()}; // TODO: avoid copying			
					}
				}, m_source);
		}

		std::shared_ptr<std::istream> istream() const
		{
			return std::make_shared<imemorystream>(span());
		}

		std::optional<std::filesystem::path> path() const
		{
			if (std::holds_alternative<std::filesystem::path>(m_source))
				return std::get<std::filesystem::path>(m_source);
			else
				return std::nullopt;
		}

		std::optional<docwire::file_extension> file_extension() const
		{
			if (m_file_extension)
				return m_file_extension;
			if (std::holds_alternative<std::filesystem::path>(m_source))
				return docwire::file_extension{std::get<std::filesystem::path>(m_source)};
			else
				return std::nullopt;
		}

		unique_identifier id() const
		{
			return m_id;
		}

		void log_to_record_stream(log_record_stream& s) const
		{
			s << docwire_log_streamable_obj(*this, m_file_extension);
		}

	private:
		std::variant<std::filesystem::path, std::vector<std::byte>, std::span<const std::byte>, std::string, std::string_view, seekable_stream_ptr, unseekable_stream_ptr> m_source;
		std::optional<docwire::file_extension> m_file_extension;
		mutable std::shared_ptr<memory_buffer> m_memory_cache;
		unique_identifier m_id;

		std::shared_ptr<memory_buffer> read_unseekable_stream_into_memory(std::shared_ptr<std::istream> stream) const
		{
			constexpr size_t chunk_size = 4096;
			auto buffer = std::make_shared<memory_buffer>(0);
			size_t size = 0;
			for (;;)
			{
				buffer->resize(size + chunk_size);
				if (!stream->read(reinterpret_cast<char*>(buffer->data() + size), chunk_size))
				if (!stream->eof())
					throw RuntimeError("Failed to read from stream using unseekable method: read() failed");
				size_t bytes_read = stream->gcount();
				size += bytes_read;
				if (bytes_read < chunk_size)
				{
					buffer->resize(size);
					break;
				}
			}
			return buffer;
		}

		std::shared_ptr<memory_buffer> read_seekable_stream_into_memory(std::shared_ptr<std::istream> stream) const
		{	
			if (!stream->seekg(0, std::ios::end))
				throw RuntimeError("Failed to read from stream using seekable method: seekg() failed");
			auto buffer = std::make_shared<memory_buffer>(stream->tellg());
			if (!stream->seekg(0, std::ios::beg))
				throw RuntimeError("Failed to read from stream using seekable method: seekg() failed");
			if (!stream->read(reinterpret_cast<char*>(buffer->data()), buffer->size()))
				throw RuntimeError("Failed to read from stream using seekable method: read() failed");
			return buffer;
		}

		void fill_memory_cache() const
		{
			if (m_memory_cache)
				return;
    		std::visit(
    			overloaded {
        			[this](const std::filesystem::path& source)
					{
          				auto stream = std::make_shared<std::ifstream>(source, std::ios::binary);
          				if (!stream->good())
	        			{
		        			docwire_log(error) << "Error opening file: " << source;
		        			throw RuntimeError("Error opening file: " + source.string());
	        			}
          				m_memory_cache = read_seekable_stream_into_memory(stream);
        			},
					[this](const std::span<const std::byte>& source)
					{
						throw LogicError("std::span cannot be cached in memory");
					},
        			[this](const std::string& source)
        			{
          				throw LogicError("std::string cannot be cached in memory");
        			},
        			[this](seekable_stream_ptr source)
					{
    					m_memory_cache = read_seekable_stream_into_memory(source.v);
        			},
        			[this](unseekable_stream_ptr source)
					{
          				m_memory_cache = read_unseekable_stream_into_memory(source.v);
        			}
      			},
	  			m_source
    		);
		};
};

} // namespace docwire

#endif // DOCWIRE_DATA_SOURCE_H
