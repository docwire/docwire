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

#ifndef DOCWIRE_CHECKED_H
#define DOCWIRE_CHECKED_H

#include "safety_policy.h"
#include "enforce.h"
#include <utility>
#include <type_traits>

namespace docwire
{

/**
 * @brief A generic wrapper for dereferenceable types (like pointers and optionals)
 * that provides checked access based on a safety_policy.
 *
 * In `strict` mode, the dereference operators (`*`, `->`) check if the object is valid
 * and throw an exception if it does not.
 * In `relaxed` mode, this wrapper is a zero-cost abstraction with no runtime checks,
 * behaving like the underlying type.
 *
 * @tparam Dereferenceable The underlying type (e.g., pointer, std::optional).
 * @tparam safety_level The safety policy to apply.
 * @sa safety_policy
 * @sa enforce
 */
template <typename Dereferenceable, safety_policy safety_level = default_safety_level>
class checked
{
public:
    /**
     * @brief Default constructor.
     */
    checked() = default;
    /**
     * @brief Constructs from a value.
     * Implicit conversion is allowed to support seamless wrapping of null-like types.
     */
    checked(const Dereferenceable& value) : m_value(value) {}
    /**
     * @brief Move-constructs from a value.
     */
    checked(Dereferenceable&& value) noexcept(std::is_nothrow_move_constructible_v<Dereferenceable>) : m_value(std::move(value)) {}

    /**
     * @brief Forwards arguments to construct the underlying object in-place.
     */
    template <typename... Args,
              typename = std::enable_if_t<
                  std::is_constructible_v<Dereferenceable, Args...> &&
                  (sizeof...(Args) > 1 || 
                   (sizeof...(Args) == 1 && 
                    ((!std::is_same_v<std::decay_t<Args>, checked> && 
                      !std::is_same_v<std::decay_t<Args>, Dereferenceable>) && ...)))
              >>
    checked(Args&&... args)
        : m_value(std::forward<Args>(args)...) {}

    checked& operator=(const Dereferenceable& value) { m_value = value; return *this; }
    checked& operator=(Dereferenceable&& value) noexcept(std::is_nothrow_move_assignable_v<Dereferenceable>) { m_value = std::move(value); return *this; }

    /// Returns a reference to the underlying value, checking for validity in strict mode.
    const auto& operator*() const
    {
        enforce<safety_level>(static_cast<bool>(m_value), "Attempted to dereference a null/empty value");
        return *m_value;
    }

    /// Returns a reference to the underlying value, checking for validity in strict mode.
    auto& operator*()
    {
        enforce<safety_level>(static_cast<bool>(m_value), "Attempted to dereference a null/empty value");
        return *m_value;
    }

    /// Returns a pointer to the underlying value, checking for validity in strict mode.
    auto operator->() const
    {
        enforce<safety_level>(static_cast<bool>(m_value), "Attempted to dereference a null/empty value");
        return &this->operator*();
    }

    // Forwarded methods
    /// Checks if the underlying value is valid (not null/empty).
    explicit operator bool() const noexcept { return static_cast<bool>(m_value); }
    /// Returns the raw underlying pointer/value without checks.
    auto get() const requires requires(const Dereferenceable& v) { v.get(); } { return m_value.get(); }

    /// Checks if the underlying object has a value.
    bool has_value() const noexcept(noexcept(std::declval<const Dereferenceable&>().has_value()))
        requires requires(const Dereferenceable& v) { v.has_value(); }
    {
        return m_value.has_value();
    }

    /// Returns the value, potentially throwing if empty (forwarding underlying behavior).
    constexpr decltype(auto) value() const
        noexcept(noexcept(std::declval<const Dereferenceable&>().value()))
        requires requires(const Dereferenceable& v) { v.value(); }
    {
        return m_value.value();
    }

    /// Returns the value, potentially throwing if empty (forwarding underlying behavior).
    constexpr decltype(auto) value()
        noexcept(noexcept(std::declval<Dereferenceable&>().value()))
        requires requires(Dereferenceable& v) { v.value(); }
    {
        return m_value.value();
    }

    /// Returns the contained value or a default value if empty.
    template <typename U>
    constexpr auto value_or(U&& default_value) const
        noexcept(noexcept(std::declval<const Dereferenceable&>().value_or(std::forward<U>(default_value))))
        requires requires(const Dereferenceable& v) { v.value_or(std::forward<U>(default_value)); }
    {
        return m_value.value_or(std::forward<U>(default_value));
    }

    /// Resets the underlying value.
    template<typename... Args>
    void reset(Args&&... args) noexcept(noexcept(std::declval<Dereferenceable&>().reset(std::forward<Args>(args)...))) requires requires(Dereferenceable& v, Args&&... a) { v.reset(std::forward<Args>(a)...); }
    {
        m_value.reset(std::forward<Args>(args)...);
    }

    /// Constructs the underlying value in-place.
    template<class... Args>
    auto& emplace(Args&&... args) requires requires(Dereferenceable& v, Args&&... a) { v.emplace(std::forward<Args>(a)...); }
    {
        return m_value.emplace(std::forward<Args>(args)...);
    }

    /// Unwraps the checked wrapper to return the underlying object.
    constexpr const Dereferenceable& unwrap() const & noexcept { return m_value; }
    constexpr Dereferenceable& unwrap() & noexcept { return m_value; }
    constexpr Dereferenceable&& unwrap() && noexcept { return std::move(m_value); }

    explicit constexpr operator const Dereferenceable&() const & noexcept { return m_value; }
    explicit constexpr operator Dereferenceable&() & noexcept { return m_value; }
    explicit constexpr operator Dereferenceable&&() && noexcept { return std::move(m_value); }

    /// Compares the underlying value with another value.
    template <typename U>
    friend constexpr bool operator==(const checked& lhs, const U& rhs)
        requires (!std::is_same_v<std::decay_t<U>, checked>) &&
                 requires { std::declval<const Dereferenceable&>() == rhs; }
    {
        return lhs.m_value == rhs;
    }

    /// Compares the underlying values of two checked wrappers.
    friend constexpr bool operator==(const checked& lhs, const checked& rhs)
        requires requires { std::declval<const Dereferenceable&>() == std::declval<const Dereferenceable&>(); }
    {
        return lhs.m_value == rhs.m_value;
    }

private:
    Dereferenceable m_value;
};

} // namespace docwire

#endif // DOCWIRE_CHECKED_H