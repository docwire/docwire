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

#ifndef DOCWIRE_VARIANT_CHAIN_ELEMENT_H
#define DOCWIRE_VARIANT_CHAIN_ELEMENT_H

#include "chain_element.h"
#include "noop_transformer.h"
#include "ref_or_owned.h"
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace docwire
{

template <typename T>
struct is_std_variant : std::false_type {};

template <typename... Ts>
struct is_std_variant<std::variant<Ts...>> : std::true_type {};

template <typename T>
concept std_variant = is_std_variant<std::remove_cvref_t<T>>::value;

template <typename T>
concept variant_alternative_invocable =
    requires(T& element, message_ptr msg, const message_callbacks& cb) {
        { element(std::move(msg), cb) } -> std::convertible_to<continuation>;
    };

namespace pipeline::detail
{

/**
 * @brief Computes the common pipeline role of a variant's alternatives.
 *
 * All alternatives must share the same role.
 */
template <typename... Ts>
struct variant_common_role
{
    static_assert(sizeof...(Ts) > 0,
                  "variant_chain_element requires at least one alternative");

    using first_alternative = std::tuple_element_t<0, std::tuple<Ts...>>;

    static constexpr role value = pipeline::role_v<first_alternative>;

    static_assert(((pipeline::role_v<Ts> == value) && ...),
                  "All variant chain elements must share the same pipeline role");
};

/**
 * @brief Maps a pipeline role to its category base.
 */
template <role R, typename Derived>
using role_base_t =
    std::conditional_t<R == role::source, source_element<Derived>,
    std::conditional_t<R == role::transformer, transformer_element<Derived>,
    std::conditional_t<R == role::consumer, consumer_element<Derived>,
    complete_pipeline<Derived>>>>;

} // namespace pipeline::detail

/**
 * @brief A pipeline element wrapping a `std::variant` of alternative chain elements.
 *
 * `variant_chain_element` allows a single, statically typed pipeline to select
 * one of several alternative chain elements at runtime without falling back to
 * dynamic dispatch. Selection is performed with `std::visit`, which the compiler
 * lowers to a tagged-union jump table and inlines a direct call per alternative.
 * This stays consistent with the "no `virtual`, no `std::function`" policy.
 *
 * All alternatives MUST share the same pipeline role (source, transformer, or
 * consumer). A variant mixing elements of different roles is rejected at
 * compile time via `static_assert`. Use `noop_transformer` as an alternative to
 * represent an absent (optional) step.
 *
 * @tparam Variant A `std::variant<Ts...>` whose alternatives are chain elements.
 *
 * @see noop_transformer
 * @see chain_element
 * @see chain_element_type
 */
template <typename Variant>
class variant_chain_element;

template <typename... Ts>
class variant_chain_element<std::variant<Ts...>>
    : public pipeline::detail::role_base_t<
          pipeline::detail::variant_common_role<Ts...>::value,
          variant_chain_element<std::variant<Ts...>>>
{
private:
    static_assert((variant_alternative_invocable<Ts> && ...),
                  "All variant alternatives must accept (message_ptr, const message_callbacks&)");

public:
    variant_chain_element() = default;

    template <typename V>
        requires std::same_as<std::remove_cvref_t<V>, std::variant<Ts...>>
    variant_chain_element(V&& value)
        : m_value{ref_or_owned<std::variant<Ts...>>{std::forward<V>(value)}}
    {
    }

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        return std::visit(
            [&](auto& element) -> continuation
            {
                return element(std::move(msg), emit_message);
            },
            m_value.get());
    }

private:
    ref_or_owned<std::variant<Ts...>> m_value;
};

template <typename... Ts>
variant_chain_element(std::variant<Ts...>)
    -> variant_chain_element<std::variant<Ts...>>;

template <typename L, typename Variant>
    requires chain_element_type<L> && std_variant<Variant>
auto operator|(L&& lhs, Variant&& rhs)
{
    using variant_type = std::remove_cvref_t<Variant>;
    return std::forward<L>(lhs)
         | variant_chain_element<variant_type>{std::forward<Variant>(rhs)};
}

template <typename Variant, typename R>
    requires std_variant<Variant> && chain_element_type<R>
auto operator|(Variant&& lhs, R&& rhs)
{
    using variant_type = std::remove_cvref_t<Variant>;
    return variant_chain_element<variant_type>{std::forward<Variant>(lhs)}
         | std::forward<R>(rhs);
}

template <typename LeftVariant, typename RightVariant>
    requires std_variant<LeftVariant> && std_variant<RightVariant>
auto operator|(LeftVariant&& lhs, RightVariant&& rhs)
{
    using lhs_variant_type = std::remove_cvref_t<LeftVariant>;
    using rhs_variant_type = std::remove_cvref_t<RightVariant>;
    return variant_chain_element<lhs_variant_type>{std::forward<LeftVariant>(lhs)}
         | variant_chain_element<rhs_variant_type>{std::forward<RightVariant>(rhs)};
}

} // namespace docwire

#endif // DOCWIRE_VARIANT_CHAIN_ELEMENT_H
