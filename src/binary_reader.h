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

#ifndef DOCWIRE_BINARY_READER_H
#define DOCWIRE_BINARY_READER_H

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <type_traits>

namespace docwire::binary
{

// Polyfill for std::byteswap, which is C++23
#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
    using std::byteswap;
#else
    template <std::integral T>
    constexpr T byteswap(T value) noexcept
    {
        if (!std::is_constant_evaluated()) {
            if constexpr (sizeof(T) == 2) return __builtin_bswap16(value);
            if constexpr (sizeof(T) == 4) return __builtin_bswap32(value);
            if constexpr (sizeof(T) == 8) return __builtin_bswap64(value);
        }
        T result{};
        auto* p_value = reinterpret_cast<std::byte*>(&value);
        auto* p_result = reinterpret_cast<std::byte*>(&result);
        for (size_t i = 0; i < sizeof(T); ++i)
            p_result[i] = p_value[sizeof(T) - 1 - i];
        return result;
    }
#endif

/**
 * @brief A simple, endian-aware reader for binary data from an abstract source.
 *
 * This reader is stateless and operates via a "pull" functor, providing a safe
 * and abstract way to parse fundamental types. It is not tied to any specific
 * stream or buffer type.
 */
class reader
{
public:
    /**
     * @brief A function that fills a destination span with bytes from a source.
     * This function is expected to throw an exception on failure (e.g., EOF).
     * @param dest The span to fill with data.
     */
    using pull_reader = std::function<void(std::span<std::byte>)>;

    /**
     * @brief Constructs a reader from a pull-style reader function.
     * @param reader The function to call to get bytes.
     */
    explicit reader(pull_reader reader) : m_reader(std::move(reader)) {}

    template <std::integral T>
    T read_little_endian()
    {
        T value;
        m_reader({reinterpret_cast<std::byte*>(&value), sizeof(T)});
        if constexpr (std::endian::native == std::endian::big)
            return byteswap(value);
        return value;
    }

    float read_float_le()
    {
        static_assert(sizeof(float) == sizeof(uint32_t));
        const uint32_t int_val = read_little_endian<uint32_t>();
        return std::bit_cast<float>(int_val);
    }

    double read_double_le()
    {
        static_assert(sizeof(double) == sizeof(uint64_t));
        const uint64_t int_val = read_little_endian<uint64_t>();
        return std::bit_cast<double>(int_val);
    }

    void read(std::span<std::byte> dest)
    {
        m_reader(dest);
    }

private:
    pull_reader m_reader;
};

} // namespace docwire::binary

#endif // DOCWIRE_BINARY_READER_H
