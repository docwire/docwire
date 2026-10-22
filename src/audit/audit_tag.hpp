#ifndef DOCWIRE_AUDIT_AUDIT_TAG_H
#define DOCWIRE_AUDIT_AUDIT_TAG_H

#include <concepts>
#include <string_view>

namespace docwire::audit
{

/**
 * @brief Concept for types that behave as audit tags.
 *
 * Audit tags identify the semantic category of an audit event in a
 * structured, type-safe way.
 */
template<typename T>
concept audit_tag = requires
{
    { T::string() } -> std::convertible_to<std::string_view>;
};

} // namespace docwire::audit

#endif
