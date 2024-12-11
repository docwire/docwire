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

#include "defines.h"
#include "file_extension.h"
#include <filesystem>
#include "memory_buffer.h"
#include <optional>
#include <string_view>
#include "unique_identifier.h"
#include <unordered_map>
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

struct mime_type
{
	std::string v;
	bool operator==(const mime_type& rhs) const = default;
};

}

namespace std {
template <>
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

enum class confidence
{
	none,
	low,
	medium,
	high,
	very_high,
	highest
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
class DllExport data_source
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

		template <data_source_compatible_type T>
		explicit data_source(const T& source, mime_type mime_type, confidence mime_type_confidence)
			: m_source{source}
		{
			add_mime_type(mime_type, mime_type_confidence);
		}

		template <data_source_compatible_type T>
		explicit data_source(T&& source, mime_type mime_type, confidence mime_type_confidence)
			: m_source{std::move(source)}
		{
			add_mime_type(mime_type, mime_type_confidence);
		}

		std::span<const std::byte> span() const;

		std::string string(std::optional<length_limit> limit = std::nullopt) const;

		std::shared_ptr<std::istream> istream() const;

		std::optional<std::filesystem::path> path() const;

		std::optional<docwire::file_extension> file_extension() const;

		unique_identifier id() const
		{
			return m_id;
		}

		std::optional<std::pair<mime_type, confidence>> highest_confidence_mime_type_info() const
		{
			auto hc_mt_it = std::max_element(mime_types.begin(), mime_types.end(),
			[](const auto& p1, const auto& p2)
				{
					return p1.second < p2.second;
				});
			if (hc_mt_it != mime_types.end())
				return *hc_mt_it;
			else
				return std::nullopt;
		}

		std::optional<mime_type> highest_confidence_mime_type() const
		{
			auto hc_mt = highest_confidence_mime_type_info();
			if (hc_mt)
				return hc_mt->first;
			else
				return std::nullopt;
		}

		confidence highest_mime_type_confidence() const
		{
			auto hc_mt = highest_confidence_mime_type_info();
			if (hc_mt)
				return hc_mt->second;
			else
				return confidence::none;
		}

		confidence mime_type_confidence(mime_type mt) const
		{
    		auto mt_iter = mime_types.find(mt);
			if (mt_iter == mime_types.end())
				return confidence::none;
			else
				return mt_iter->second;
		}

		void add_mime_type(mime_type mt, confidence c)
		{
			auto [existing_it, inserted] = mime_types.try_emplace(mt, c);
			if (!inserted && existing_it->second < c)
				existing_it->second = c;
		}

		std::unordered_map<mime_type, confidence> mime_types;

	private:
		std::variant<std::filesystem::path, std::vector<std::byte>, std::span<const std::byte>, std::string, std::string_view, seekable_stream_ptr, unseekable_stream_ptr> m_source;
		std::optional<docwire::file_extension> m_file_extension;
		mutable std::shared_ptr<memory_buffer> m_memory_cache;
		unique_identifier m_id;

		void fill_memory_cache() const;
};

} // namespace docwire

#endif // DOCWIRE_DATA_SOURCE_H
