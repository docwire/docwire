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

#ifndef DOCWIRE_SOURCE_LOCATION_H
#define DOCWIRE_SOURCE_LOCATION_H

#if __has_include(<source_location>) && (!defined(__clang__) || __clang_major__ >= 16) // https://github.com/llvm/llvm-project/issues/56379
	#define USE_STD_SOURCE_LOCATION 1
#else
	#warning "Cannot use std::source_location, falling back to custom implementation. For best performance, use a C++20 compliant compiler that fully supports std::source_location (e.g., GCC 11+, Clang 16+, MSVC 19.29+)."
	#define USE_STD_SOURCE_LOCATION 0
#endif

#include <cstdint>
#if USE_STD_SOURCE_LOCATION
	#include <source_location>
#endif

namespace docwire {

/// @brief A fallback implementation of source_location for compilers that do not support std::source_location.
struct basic_source_location
{
public:
    static constexpr basic_source_location current(
        const char* file = __builtin_FILE(),
        const char* function = __builtin_FUNCTION(),
        const std::uint_least32_t line = __builtin_LINE()) noexcept
    {
        return basic_source_location(file, function, line);
    }

    constexpr basic_source_location() noexcept = default;

    constexpr const char* file_name() const noexcept { return m_file; }
    constexpr const char* function_name() const noexcept { return m_function; }
    constexpr std::uint_least32_t line() const noexcept { return m_line; }
    constexpr std::uint_least32_t column() const noexcept { return 0; }

private:
    constexpr basic_source_location(const char* file, const char* function, std::uint_least32_t line) noexcept
        : m_file(file), m_function(function), m_line(line)
    {}

    const char* m_file = "";
    const char* m_function = "";
    std::uint_least32_t m_line{};
};

#if USE_STD_SOURCE_LOCATION
    /// @brief A type that describes a location in source code. Uses std::source_location when available.
    using source_location = std::source_location;
#else
    /// @brief A type that describes a location in source code. Uses a custom fallback implementation.
    using source_location = basic_source_location;
#endif

} // namespace docwire

#undef USE_STD_SOURCE_LOCATION

#endif // DOCWIRE_SOURCE_LOCATION_H
