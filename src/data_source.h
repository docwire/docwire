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

#ifndef DOCWIRE_DATA_SOURCE_H
#define DOCWIRE_DATA_SOURCE_H

#include "core_export.h"
#include "file_extension.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <istream>
#include <memory>
#include <span>
#include "memory_buffer.h"
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "error_tags.h"
#include "memorystream.h"
#include "serialization_filesystem.h"
#include "throw_if.h"
#include "unique_identifier.h"
#include <unordered_map>
#include <variant>
#include <vector>

namespace docwire
{

/// Wrapper for a shared pointer to a seekable input stream.
struct seekable_stream_ptr
{
  std::shared_ptr<std::istream> v;
};

/// Wrapper for a shared pointer to an unseekable input stream.
struct unseekable_stream_ptr
{
  std::shared_ptr<std::istream> v;
};

/// Wrapper for a length limit value.
struct length_limit
{
	size_t v;
};

/// Wrapper for a MIME type string.
struct mime_type
{
	std::string v;
	bool operator==(const mime_type& rhs) const = default;
};

}

namespace std {
template <>
/**
 * @brief Specialization of std::hash for docwire::mime_type.
 */
struct hash<docwire::mime_type>
{
	size_t operator()(const docwire::mime_type& mt) const
	{
		return hash<std::string>{}(mt.v);
	}
};
} // namespace std

namespace docwire
{

/**
 * @brief Represents the confidence level of a detected MIME type.
 */
enum class confidence
{
	none,
	low,
	medium,
	high,
	very_high,
	highest
};

/**
 * @brief Concept matching types that can be used to initialize a data_source.
 */
template <typename T>
concept data_source_compatible_type =
	std::is_same_v<T, std::filesystem::path> ||
	std::is_same_v<T, std::vector<std::byte>> ||
	std::is_same_v<T, std::span<const std::byte>> ||
	std::is_same_v<T, std::string> ||
	std::is_same_v<T, std::string_view> ||
	std::is_same_v<T, seekable_stream_ptr> ||
	std::is_same_v<T, unseekable_stream_ptr>;

/**
 * @brief Concept matching reference-qualified types compatible with data_source.
 */
template <typename T>
concept data_source_compatible_type_ref_qualified = data_source_compatible_type<std::remove_reference_t<T>>;

/**
 * @brief A helper for creating a visitor from a set of lambdas, used for visiting `std::variant`.
 * @tparam Ts The lambda types.
 */
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// Deduction guide for `overloaded`.
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
		/**
		 * @brief Constructs a data_source from a compatible type.
		 * @param source The data source (e.g., path, string, vector<byte>).
		 */
		template <data_source_compatible_type T>
		explicit data_source(const T& source)
			: m_source{source}
		{}

		/**
		 * @brief Constructs a data_source by moving from a compatible type.
		 * @param source The data source to move from.
		 */
		template <data_source_compatible_type T>
		explicit data_source(T&& source)
			: m_source{std::move(source)}
		{}

		/**
		 * @brief Constructs a data_source with an explicit file extension.
		 * @param source The data source.
		 * @param file_extension The file extension to associate with the data.
		 */
		template <data_source_compatible_type T>
		explicit data_source(const T& source, file_extension file_extension)
			: m_source{source}, m_file_extension{file_extension}
		{}

		/**
		 * @brief Constructs a data_source by moving, with an explicit file extension.
		 * @param source The data source to move from.
		 * @param file_extension The file extension to associate with the data.
		 */
		template <data_source_compatible_type T>
		explicit data_source(T&& source, file_extension file_extension)
			: m_source{std::move(source)}, m_file_extension{file_extension}
		{}

		/**
		 * @brief Constructs a data_source with an initial MIME type and confidence.
		 * @param source The data source.
		 * @param mime_type The initial MIME type.
		 * @param mime_type_confidence The confidence level for the initial MIME type.
		 */
		template <data_source_compatible_type T>
		explicit data_source(const T& source, mime_type mime_type, confidence mime_type_confidence)
			: m_source{source}
		{
			add_mime_type(mime_type, mime_type_confidence);
		}

		/**
		 * @brief Constructs a data_source by moving, with an initial MIME type and confidence.
		 * @param source The data source to move from.
		 * @param mime_type The initial MIME type.
		 * @param mime_type_confidence The confidence level for the initial MIME type.
		 */
		template <data_source_compatible_type T>
		explicit data_source(T&& source, mime_type mime_type, confidence mime_type_confidence)
			: m_source{std::move(source)}
		{
			add_mime_type(mime_type, mime_type_confidence);
		}

		/**
		 * @brief Returns the content as a span of bytes.
		 * @param limit Optional limit on the number of bytes to return.
		 * @return A span over the data.
		 */
		std::span<const std::byte> span(std::optional<length_limit> limit = std::nullopt) const;

		/**
		 * @brief Returns the content as a string.
		 * @param limit Optional limit on the number of characters to return.
		 * @return A string containing the data.
		 */
		std::string string(std::optional<length_limit> limit = std::nullopt) const;

		/**
		 * @brief Returns the content as a string_view.
		 * 
		 * This method avoids memory allocation if the underlying source is already in memory
		 * (e.g. string, vector<byte>). If the source is a stream or file, it may load data.
		 * 
		 * @param limit Optional limit on the number of characters to return.
		 */
		std::string_view string_view(std::optional<length_limit> limit = std::nullopt) const;

		/// Returns an input stream for reading the data.
		std::shared_ptr<std::istream> istream() const;

		/// Returns the file path if the source is a file, otherwise std::nullopt.
		std::optional<std::filesystem::path> path() const;

		/// Returns the file extension if available.
		std::optional<docwire::file_extension> file_extension() const;

		/// Returns the unique identifier for this data source.
		unique_identifier id() const
		{
			return m_id;
		}

		/**
		 * @brief Returns the MIME type with the highest confidence and its confidence level.
		 * 
		 * @note Because a file extension can map to multiple valid MIME type aliases with the 
		 * same confidence level, this method uses a deterministic alphabetical tie-breaker 
		 * (e.g., `application/xml` wins over `text/xml`) to guarantee consistent cross-platform behavior.
		 */
		std::optional<std::pair<mime_type, confidence>> highest_confidence_mime_type_info() const
		{
			auto hc_mt_it = std::max_element(mime_types.begin(), mime_types.end(),
			[](const auto& p1, const auto& p2)
				{
					if (p1.second != p2.second)
						return p1.second < p2.second;
					// Deterministic tie-breaker: alphabetically earlier string wins.
					// Since max_element looks for the "largest" element, we invert the string comparison.
					return p1.first.v > p2.first.v;
				});
			if (hc_mt_it != mime_types.end())
				return *hc_mt_it;
			else
				return std::nullopt;
		}

		/// Returns the MIME type with the highest confidence.
		std::optional<mime_type> highest_confidence_mime_type() const
		{
			auto hc_mt = highest_confidence_mime_type_info();
			if (hc_mt)
				return hc_mt->first;
			else
				return std::nullopt;
		}

		/// Returns the highest confidence level found among detected MIME types.
		confidence highest_mime_type_confidence() const
		{
			auto hc_mt = highest_confidence_mime_type_info();
			if (hc_mt)
				return hc_mt->second;
			else
				return confidence::none;
		}

		/**
		 * @brief Checks if the highest confidence mime type is present in the given list.
		 * @param mts The list of mime types to check against.
		 */
		bool has_highest_confidence_mime_type_in(const std::vector<mime_type>& mts) const;
		/// Asserts that the data source is not encrypted.
		void assert_not_encrypted() const;

		/// Returns the confidence level for a specific MIME type.
		confidence mime_type_confidence(mime_type mt) const
		{
    		auto mt_iter = mime_types.find(mt);
			if (mt_iter == mime_types.end())
				return confidence::none;
			else
				return mt_iter->second;
		}

		/**
		 * @brief Adds a mime type with a confidence level.
		 * @param mt The mime type to add.
		 * @param c The confidence level.
		 */
		void add_mime_type(mime_type mt, confidence c)
		{
			auto [existing_it, inserted] = mime_types.try_emplace(mt, c);
			if (!inserted && existing_it->second < c)
				existing_it->second = c;
		}

		/// Map of detected MIME types and their confidence levels.
		std::unordered_map<mime_type, confidence> mime_types;

	private:
		std::variant<std::filesystem::path, std::vector<std::byte>, std::span<const std::byte>, std::string, std::string_view, seekable_stream_ptr, unseekable_stream_ptr> m_source;
		std::optional<docwire::file_extension> m_file_extension;
		mutable std::shared_ptr<memory_buffer> m_memory_cache;
		mutable std::shared_ptr<std::istream> m_path_stream;
		mutable std::optional<size_t> m_stream_size;
		unique_identifier m_id;

		void fill_memory_cache(std::optional<length_limit> limit) const;
};

namespace detail
{

inline void read_unseekable_stream_into_memory(std::shared_ptr<memory_buffer> buffer, std::shared_ptr<std::istream> stream, std::optional<length_limit> limit)
{
	constexpr size_t chunk_size = 4096;
	size_t size = buffer->size();
	for (;;)
	{
		if (limit && size >= limit->v)
			break;
		size_t to_read = limit ? (std::min)(chunk_size, limit->v - size) : chunk_size;
		buffer->resize(size + to_read);
		DOCWIRE_THROW_IF (!stream->read(reinterpret_cast<char*>(buffer->data() + size), to_read) && !stream->eof());
		size_t bytes_read = stream->gcount();
		size += bytes_read;
		if (bytes_read < to_read)
		{
			buffer->resize(size);
			break;
		}
	}
}

inline void read_seekable_stream_into_memory(std::shared_ptr<memory_buffer> buffer, std::optional<size_t>& stream_size, std::shared_ptr<std::istream> stream, std::optional<length_limit> limit)
{
	if (!stream_size)
	{
		DOCWIRE_THROW_IF (!stream->seekg(0, std::ios::end));
		stream_size = stream->tellg();
		DOCWIRE_THROW_IF (!stream->seekg(0, std::ios::beg));
	}
	size_t size = buffer->size();
	if ((limit ? (std::min)(*stream_size, limit->v) : *stream_size) <= size)
		return;
	size_t to_read = (limit ? (std::min)(*stream_size, limit->v) : *stream_size) - size;
	buffer->resize(size + to_read);
	DOCWIRE_THROW_IF (!stream->read(reinterpret_cast<char*>(buffer->data() + size), to_read));
}

} // namespace detail

inline std::span<const std::byte> data_source::span(std::optional<length_limit> limit) const
{
	return std::visit(
		overloaded {
			[this, limit](const std::vector<std::byte>& source)
			{
				size_t size = limit ? (std::min)(source.size(), limit->v) : source.size();
				return std::span{source.data(), size};
			},
			[this, limit](const std::span<const std::byte>& source)
			{
				size_t size = limit ? (std::min)(source.size(), limit->v) : source.size();
				return std::span{source.data(), size};
			},
			[this, limit](const std::string& source)
			{
				size_t size = limit ? (std::min)(source.size(), limit->v) : source.size();
				return std::span{reinterpret_cast<const std::byte*>(source.data()), size};
			},
			[this, limit](const std::string_view& source)
			{
				size_t size = limit ? (std::min)(source.size(), limit->v) : source.size();
				return std::span{reinterpret_cast<const std::byte*>(source.data()), size};
			},
			[this, limit](auto source)
			{
				fill_memory_cache(limit);
				size_t size = limit ? (std::min)(m_memory_cache->size(), limit->v) : m_memory_cache->size();
				return std::span<const std::byte>(m_memory_cache->data(), size);
			}
		}, m_source);
}

inline std::string data_source::string(std::optional<length_limit> limit) const
{
	return std::visit(
		overloaded {
			[this, limit](const std::vector<std::byte>& source)
			{
				if (limit)
					return std::string{reinterpret_cast<const char*>(source.data()), (std::min)(source.size(), limit->v)};
				else
					return std::string{reinterpret_cast<const char*>(source.data()), source.size()};
			},
			[this, limit](const std::span<const std::byte>& source)
			{
				if (limit)
					return std::string{reinterpret_cast<const char*>(source.data()), (std::min)(source.size(), limit->v)};
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
					return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), (std::min)(m_memory_cache->size(), limit->v)};
				else
					return std::string{reinterpret_cast<const char*>(m_memory_cache->data()), m_memory_cache->size()};
			}
		}, m_source);
}

inline std::string_view data_source::string_view(std::optional<length_limit> limit) const
{
	return std::visit(
		overloaded {
			[this, limit](const std::string& source)
			{
				return std::string_view{source}.substr(0, limit ? limit->v : std::string_view::npos);
			},
			[this, limit](const std::string_view& source)
			{
				return source.substr(0, limit ? limit->v : std::string_view::npos);
			},
			[this, limit](const auto& source)
			{
				std::span<const std::byte> data_span = span(limit);
				return std::string_view{reinterpret_cast<const char*>(data_span.data()), data_span.size()};
			}
		}, m_source);
}

inline std::shared_ptr<std::istream> data_source::istream() const
{
	return std::make_shared<imemorystream>(span());
}

inline std::optional<std::filesystem::path> data_source::path() const
{
	if (std::holds_alternative<std::filesystem::path>(m_source))
		return std::get<std::filesystem::path>(m_source);
	else
		return std::nullopt;
}

inline std::optional<docwire::file_extension> data_source::file_extension() const
{
	if (m_file_extension)
		return m_file_extension;
	if (std::holds_alternative<std::filesystem::path>(m_source))
		return docwire::file_extension{std::get<std::filesystem::path>(m_source)};
	else
		return std::nullopt;
}

inline bool data_source::has_highest_confidence_mime_type_in(const std::vector<mime_type>& mts) const
{
	std::optional<mime_type> mt = highest_confidence_mime_type();
	DOCWIRE_THROW_IF(!mt, "Data source has no mime type", errors::uninterpretable_data{});
	return std::find(mts.begin(), mts.end(), *mt) != mts.end();
}

inline void data_source::assert_not_encrypted() const
{
	bool is_encrypted = mime_type_confidence(mime_type { "application/encrypted" }) >= confidence::high;
	DOCWIRE_THROW_IF(is_encrypted, errors::file_encrypted{});
}

inline void data_source::fill_memory_cache(std::optional<length_limit> limit) const
{
	std::visit(
		overloaded {
			[this, limit](const std::filesystem::path& source)
			{
				if (!m_memory_cache)
				{
					m_path_stream = std::make_shared<std::ifstream>(source, std::ios::binary);
					DOCWIRE_THROW_IF (!m_path_stream->good(), source);
					m_memory_cache = std::make_shared<memory_buffer>(0);
				}
				detail::read_seekable_stream_into_memory(m_memory_cache, m_stream_size, m_path_stream, limit);
			},
			[this](const std::span<const std::byte>& source)
			{
				throw DOCWIRE_MAKE_ERROR("std::span cannot be cached in memory", errors::program_logic{});
			},
			[this](const std::string& source)
			{
				throw DOCWIRE_MAKE_ERROR("std::string cannot be cached in memory", errors::program_logic{});
			},
			[this, limit](seekable_stream_ptr source)
			{
				if (!m_memory_cache)
					m_memory_cache = std::make_shared<memory_buffer>(0);
				detail::read_seekable_stream_into_memory(m_memory_cache, m_stream_size, source.v, limit);
			},
			[this, limit](unseekable_stream_ptr source)
			{
				if (!m_memory_cache)
					m_memory_cache = std::make_shared<memory_buffer>(0);
				detail::read_unseekable_stream_into_memory(m_memory_cache, source.v, limit);
			}
		},
		m_source
	);
}

} // namespace docwire

#endif // DOCWIRE_DATA_SOURCE_H
