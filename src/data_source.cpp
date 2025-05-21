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

#include "data_source.h"

#include "error_tags.h"
#include <fstream>
#include "memorystream.h"
#include "throw_if.h"

namespace docwire
{

std::span<const std::byte> data_source::span(std::optional<length_limit> limit) const
{
	return std::visit(
		overloaded {
			[this, limit](const std::vector<std::byte>& source)
			{
				size_t size = limit ? std::min(source.size(), limit->v) : source.size();
				return std::span{source.data(), size};
			},
			[this, limit](const std::span<const std::byte>& source)
			{
				size_t size = limit ? std::min(source.size(), limit->v) : source.size();
				return std::span{source.data(), size};
			},
			[this, limit](const std::string& source)
			{
				size_t size = limit ? std::min(source.size(), limit->v) : source.size();
				return std::span{reinterpret_cast<const std::byte*>(source.data()), size};
			},
			[this, limit](const std::string_view& source)
			{
				size_t size = limit ? std::min(source.size(), limit->v) : source.size();
				return std::span{reinterpret_cast<const std::byte*>(source.data()), size};
			},
			[this, limit](auto source)
			{
				fill_memory_cache(limit);
				size_t size = limit ? std::min(m_memory_cache->size(), limit->v) : m_memory_cache->size();
				return std::span<const std::byte>(m_memory_cache->data(), size);
			}
		}, m_source);
}

std::string data_source::string(std::optional<length_limit> limit) const
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
				fill_memory_cache(limit);
				if (limit)
					return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), std::min(m_memory_cache->size(), limit->v)};
				else
					return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), m_memory_cache->size()}; // TODO: avoid copying			
			}
		}, m_source);
}

std::shared_ptr<std::istream> data_source::istream() const
{
	return std::make_shared<imemorystream>(span());
}

std::optional<std::filesystem::path> data_source::path() const
{
	if (std::holds_alternative<std::filesystem::path>(m_source))
		return std::get<std::filesystem::path>(m_source);
	else
		return std::nullopt;
}

std::optional<docwire::file_extension> data_source::file_extension() const
{
	if (m_file_extension)
		return m_file_extension;
	if (std::holds_alternative<std::filesystem::path>(m_source))
		return docwire::file_extension{std::get<std::filesystem::path>(m_source)};
	else
		return std::nullopt;
}

namespace
{

void read_unseekable_stream_into_memory(std::shared_ptr<memory_buffer> buffer, std::shared_ptr<std::istream> stream, std::optional<length_limit> limit)
{
	constexpr size_t chunk_size = 4096;
	size_t size = buffer->size();
	for (;;)
	{
		if (limit && size >= limit->v)
			break;
		size_t to_read = limit ? std::min(chunk_size, limit->v - size) : chunk_size;
		buffer->resize(size + to_read);
		throw_if (!stream->read(reinterpret_cast<char*>(buffer->data() + size), to_read) && !stream->eof());
		size_t bytes_read = stream->gcount();
		size += bytes_read;
		if (bytes_read < to_read)
		{
			buffer->resize(size);
			break;
		}
	}
}

void read_seekable_stream_into_memory(std::shared_ptr<memory_buffer> buffer, std::optional<size_t>& stream_size, std::shared_ptr<std::istream> stream, std::optional<length_limit> limit)
{
	if (!stream_size)
	{
		throw_if (!stream->seekg(0, std::ios::end));
		stream_size = stream->tellg();
		throw_if (!stream->seekg(0, std::ios::beg));
	}
	size_t size = buffer->size();
	if ((limit ? std::min(*stream_size, limit->v) : *stream_size) <= size)
		return;
	size_t to_read = (limit ? std::min(*stream_size, limit->v) : *stream_size) - size;
	buffer->resize(size + to_read);
	throw_if (!stream->read(reinterpret_cast<char*>(buffer->data() + size), to_read));
}

} // anonymous namespace

bool data_source::has_highest_confidence_mime_type_in(const std::vector<mime_type>& mts) const
{
	std::optional<mime_type> mt = highest_confidence_mime_type();
	throw_if(!mt, "Data source has no mime type", errors::uninterpretable_data{});
	return std::find(mts.begin(), mts.end(), *mt) != mts.end();
}

void data_source::assert_not_encrypted() const
{
	bool is_encrypted = mime_type_confidence(mime_type { "application/encrypted" }) >= confidence::high;
	throw_if(is_encrypted, errors::file_encrypted{});
}

void data_source::fill_memory_cache(std::optional<length_limit> limit) const
{
	std::visit(
		overloaded {
			[this, limit](const std::filesystem::path& source)
			{
				if (!m_memory_cache)
				{
					m_path_stream = std::make_shared<std::ifstream>(source, std::ios::binary);
					throw_if (!m_path_stream->good(), source);
					m_memory_cache = std::make_shared<memory_buffer>(0);
				}
				read_seekable_stream_into_memory(m_memory_cache, m_stream_size, m_path_stream, limit);
			},
			[this](const std::span<const std::byte>& source)
			{
				throw make_error("std::span cannot be cached in memory", errors::program_logic{});
			},
			[this](const std::string& source)
			{
				throw make_error("std::string cannot be cached in memory", errors::program_logic{});
			},
			[this, limit](seekable_stream_ptr source)
			{
				if (!m_memory_cache)
					m_memory_cache = std::make_shared<memory_buffer>(0);
				read_seekable_stream_into_memory(m_memory_cache, m_stream_size, source.v, limit);
			},
			[this, limit](unseekable_stream_ptr source)
			{
				if (!m_memory_cache)
					m_memory_cache = std::make_shared<memory_buffer>(0);
				read_unseekable_stream_into_memory(m_memory_cache, source.v, limit);
			}
		},
		m_source
	);
};

} // namespace docwire
