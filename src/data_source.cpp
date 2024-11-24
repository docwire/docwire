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

std::span<const std::byte> data_source::span() const
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
				fill_memory_cache();
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

std::shared_ptr<memory_buffer> read_unseekable_stream_into_memory(std::shared_ptr<std::istream> stream)
{
	constexpr size_t chunk_size = 4096;
	auto buffer = std::make_shared<memory_buffer>(0);
	size_t size = 0;
	for (;;)
	{
		buffer->resize(size + chunk_size);
		throw_if (!stream->read(reinterpret_cast<char*>(buffer->data() + size), chunk_size) && !stream->eof());
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

std::shared_ptr<memory_buffer> read_seekable_stream_into_memory(std::shared_ptr<std::istream> stream)
{	
	throw_if (!stream->seekg(0, std::ios::end));
	auto buffer = std::make_shared<memory_buffer>(stream->tellg());
	throw_if (!stream->seekg(0, std::ios::beg));
	throw_if (!stream->read(reinterpret_cast<char*>(buffer->data()), buffer->size()));
	return buffer;
}

} // anonymous namespace

void data_source::fill_memory_cache() const
{
	if (m_memory_cache)
		return;
	std::visit(
		overloaded {
			[this](const std::filesystem::path& source)
			{
				auto stream = std::make_shared<std::ifstream>(source, std::ios::binary);
				throw_if (!stream->good());
				m_memory_cache = read_seekable_stream_into_memory(stream);
			},
			[this](const std::span<const std::byte>& source)
			{
				throw make_error("std::span cannot be cached in memory", errors::program_logic{});
			},
			[this](const std::string& source)
			{
				throw make_error("std::string cannot be cached in memory", errors::program_logic{});
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

} // namespace docwire
