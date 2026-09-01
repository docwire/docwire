#ifndef DOCWIRE_TYPE_ID_HPP
#define DOCWIRE_TYPE_ID_HPP

#include <compare>
#include <cstdint>
#include <string_view>

namespace docwire
{

/**
 * @brief Strong type representing a compile-time type identity.
 *
 * @note Value is deterministic across translation units and DLL boundaries.
 * @note The identifier is independent of std::type_info and exported RTTI objects.
 */
class type_id
{
public:
    constexpr explicit type_id(std::uint64_t value) noexcept
        : m_value{value}
    {}

    constexpr std::uint64_t value() const noexcept { return m_value; }

    constexpr auto operator<=>(const type_id&) const noexcept = default;

private:
    std::uint64_t m_value;
};

namespace detail
{

/**
 * @internal
 * @brief FNV-1a 64-bit hash algorithm.
 *
 * FNV-1a is a standard non-cryptographic hash. It is used here only to
 * compress the compiler-generated type signature into a stable 64-bit ID.
 */
constexpr std::uint64_t fnv1a_64(std::string_view str) noexcept
{
    constexpr std::uint64_t fnv_offset_basis = 14695981039346656037ull;
    constexpr std::uint64_t fnv_prime = 1099511628211ull;

    std::uint64_t hash = fnv_offset_basis;
    for (char c : str)
    {
        hash ^= static_cast<unsigned char>(c);
        hash *= fnv_prime;
    }
    return hash;
}

} // namespace detail

/**
 * @brief Returns a compile-time strong identifier for type T.
 *
 * @tparam T The type to identify.
 * @return A stable, header-only type identity usable across DLL boundaries.
 *
 * @note The ID is computed from the compiler-specific type signature:
 *       __FUNCSIG__ on MSVC and __PRETTY_FUNCTION__ on GCC/Clang.
 */
template <typename T>
consteval type_id type_id_of() noexcept
{
#ifdef _MSC_VER
    constexpr std::string_view signature = __FUNCSIG__;
#else
    constexpr std::string_view signature = __PRETTY_FUNCTION__;
#endif
    return type_id{detail::fnv1a_64(signature)};
}

} // namespace docwire

#endif // DOCWIRE_TYPE_ID_HPP
