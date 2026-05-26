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

#ifndef DOCWIRE_NOT_NULL_H
#define DOCWIRE_NOT_NULL_H

#include "safety_policy.h"
#include "enforce.h"
#include <utility>
#include <type_traits>

namespace docwire
{

/// A tag to indicate that a pointer is guaranteed to be non-null, bypassing the runtime check in not_null's constructor.
struct guaranteed_t {};

/// A constant to use with the unchecked not_null constructor, e.g., `not_null(ptr, guaranteed)`.
constexpr inline guaranteed_t guaranteed;

/**
 * @brief A wrapper for pointer-like types that enforces a non-null invariant.
 *
 * In `strict` mode, the constructor checks for null and throws if the invariant is violated.
 * In `relaxed` mode, this wrapper is a zero-cost abstraction with no runtime checks.
 *
 * @tparam Ptr The underlying pointer type.
 * @tparam safety_level The safety policy to apply.
 * @sa checked
 */
template <typename Ptr, safety_policy safety_level = default_safety_level>
class not_null
{
public:
    /**
     * @brief Constructs from a pointer, enforcing the non-null invariant in strict mode.
     */
    not_null(Ptr p) : m_ptr(std::move(p))
    {
        enforce<safety_level>(m_ptr != nullptr, "not_null constructed with a null pointer.");
    }

    /**
     * @brief Unchecked constructor for when the pointer is guaranteed to be non-null.
     * This is an optimization to avoid redundant checks, e.g., after `std::make_shared`.
     */
    not_null(Ptr p, guaranteed_t) : m_ptr(std::move(p)) {}

    // Generic forwarding constructor for constructing the underlying pointer.
    // This is constrained to avoid interfering with copy/move constructors.
    template <typename... Args,
              typename = std::enable_if_t<std::is_constructible_v<Ptr, Args...> && (sizeof...(Args) > 1)>>
    not_null(Args&&... args)
        : m_ptr(std::forward<Args>(args)...)
    {
        enforce<safety_level>(m_ptr != nullptr, "not_null constructed with a null pointer.");
    }

    // Deleted constructors to prevent creation from nullptr.
    not_null(std::nullptr_t) = delete;
    not_null& operator=(std::nullptr_t) = delete;

    /// Returns the raw pointer.
    auto get() const requires requires(const Ptr& p) { p.get(); } { return m_ptr.get(); }
    auto get() requires requires(Ptr& p) { p.get(); } { return m_ptr.get(); }

    // Dereference operators for smart-pointer-like behavior.
    auto& operator*() const { return *m_ptr; }
    auto& operator*() { return *m_ptr; }

    auto operator->() const
    {
        return &this->operator*();
    }
    auto operator->()
    {
        return &this->operator*();
    }

    explicit operator const Ptr&() const { return m_ptr; }

private:
    Ptr m_ptr;
};

/**
 * @brief Wraps a pointer-like object in a not_null, bypassing the runtime check.
 *
 * This should only be used when the pointer is guaranteed to be non-null,
 * for example, when it's the result of a factory like std::make_shared
 * which throws on failure instead of returning null.
 */
template <typename Ptr>
[[nodiscard]] not_null<std::remove_cvref_t<Ptr>> assume_not_null(Ptr&& ptr)
{
    return not_null<std::remove_cvref_t<Ptr>>(std::forward<Ptr>(ptr), guaranteed);
}

} // namespace docwire

#endif // DOCWIRE_NOT_NULL_H