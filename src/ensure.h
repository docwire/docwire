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

#ifndef DOCWIRE_ENSURE_H
#define DOCWIRE_ENSURE_H

#include <initializer_list>
#include <cassert>
#include "source_location.h"
#include "throw_if.h"
#include <vector>

namespace docwire
{

/**
 * @brief A utility for creating expressive, exception-throwing assertions in a fluent style.
 * @details This class is the core of a fluent validation API. It is designed to be
 * constructed with a value to be tested, and then used with a comparison operator
 * to check a condition. If the condition fails, it throws a `docwire::error` with
 * rich diagnostic information, including the source location of the check and the
 * values involved.
 *
 * The intended usage is natural and expressive:
 * @code
 * docwire::ensure(actual_value) == expected_value;
 * docwire::ensure(similarity_score) > 0.9;
 * docwire::ensure(my_string).contains("substring");
 * @endcode
 *
 * To prevent accidental misuse where a check is written but no comparison is performed
 * (e.g., `docwire::ensure(a == b);`), the class is marked `[[nodiscard]]` to encourage
 * compiler warnings. More importantly, in debug builds, its destructor will `assert`
 * if no comparison operator was called, immediately flagging the bug at runtime.
 * This runtime check is compiled out in release builds for zero performance overhead.
 *
 * @par Performance
 * In release builds (when `NDEBUG` is defined), this class is a true zero-cost abstraction.
 * Compilers like GCC, Clang, and MSVC with standard optimizations (e.g., `-O2` or `/O2`)
 * will inline the constructor and operators, completely optimizing away the temporary `ensure` object.
 * The resulting machine code for a check like `ensure(a) == b;` is identical to a handwritten
 * `if (!(a == b)) throw ...;`, imposing no overhead on the success path.
 *
 * @tparam T The type of the value being held for comparison.
 */
template<typename T>
class [[nodiscard]] ensure
{
public:
    /**
     * @brief Constructs an `ensure` object, capturing a value and the source location of the call.
     * @param value The value to be tested (the left-hand side of a future comparison).
     * @param loc The source location, captured automatically by the compiler.
     */
    explicit ensure(const T& value, const source_location& loc = source_location::current())
        : m_value(value), m_location(loc)
#ifndef NDEBUG
        , m_comparison_performed(false)
#endif
    {}

    ~ensure()
    {
        // In debug builds, assert that a comparison operator was used. This prevents incorrect
        // usage like `ensure(a == b);` from silently passing. The assert macro is automatically
        // compiled out in release builds (when NDEBUG is defined), resulting in zero overhead.
        assert(m_comparison_performed && "docwire::ensure() was called without a comparison operator (e.g., ==, !=, <, etc.). "
                                         "This is a bug in the calling code, not a runtime error.");
    }

    /**
     * @brief Performs an equality check (`==`). Throws if `m_value != other`.
     * @param other The value to compare against (the right-hand side).
     */
    template<typename U>
    void operator==(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value == other), m_location, m_value, other);
    }

    /**
     * @brief Performs an inequality check (`!=`). Throws if `m_value == other`.
     * @param other The value to compare against.
     */
    template<typename U>
    void operator!=(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value != other), m_location, m_value, other);
    }

    /**
     * @brief Performs a greater-than check (`>`). Throws if `m_value <= other`.
     * @param other The value to compare against.
     */
    template<typename U>
    void operator>(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value > other), m_location, m_value, other);
    }

    /**
     * @brief Performs a greater-than-or-equal-to check (`>=`). Throws if `m_value < other`.
     * @param other The value to compare against.
     */
    template<typename U>
    void operator>=(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value >= other), m_location, m_value, other);
    }

    /**
     * @brief Performs a less-than check (`<`). Throws if `m_value >= other`.
     * @param other The value to compare against.
     */
    template<typename U>
    void operator<(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value < other), m_location, m_value, other);
    }

    /**
     * @brief Performs a less-than-or-equal-to check (`<=`). Throws if `m_value > other`.
     * @param other The value to compare against.
     */
    template<typename U>
    void operator<=(const U& other) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(!(m_value <= other), m_location, m_value, other);
    }

    /**
     * @brief Checks if the held string-like value contains a substring. Throws if it does not.
     * @details This method is only available if both the held type `T` and the argument type `U`
     * are string-like (e.g., `std::string`, `std::string_view`, `const char*`).
     * @param substring The substring to search for.
     */
    template<typename U>
    requires string_like<T> && string_like<U>
    void contains(const U& substring) const
    {
        set_comparison_performed();
        DOCWIRE_THROW_IF_AT_LOCATION(std::string_view(m_value).find(substring) == std::string_view::npos, m_location, m_value, substring);
    }

    /**
     * @brief Checks if the held value is present in a given set of values. Throws if it is not.
     * @details This provides a convenient way to check against a list of acceptable outcomes.
     * @code
     * ensure(status_code).is_one_of({200, 201, 204});
     * @endcode
     * @param expected_values An initializer list of values to check against.
     */
    void is_one_of(std::initializer_list<T> expected_values) const
    {
        set_comparison_performed();
        for (const auto& expected : expected_values)
        {
            if (m_value == expected)
            {
                return; // Match found, success.
            }
        }
        DOCWIRE_THROW_IF_AT_LOCATION(true, m_location, m_value, std::vector<T>(expected_values));
    }
private:
    /**
     * @brief Marks that a comparison has been performed on this object.
     * @details This is used by the destructor in debug builds to detect incorrect usage.
     * In release builds, this function is empty and compiled out.
     */
    void set_comparison_performed() const
    {
#ifndef NDEBUG
        m_comparison_performed = true;
#endif
    }

    const T& m_value;
    docwire::source_location m_location;
#ifndef NDEBUG
    mutable bool m_comparison_performed;
#endif
};

/**
 * @brief Deduction guide for the `ensure` class template.
 * @details This allows the compiler to deduce the template argument `T` from the constructor call,
 * enabling the clean `ensure(value)` syntax without needing a factory function.
 */
template<typename T>
ensure(const T&, const docwire::source_location&) -> ensure<T>;

} // namespace docwire

#endif // DOCWIRE_ENSURE_H
