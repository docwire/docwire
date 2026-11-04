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

#ifndef DOCWIRE_PIPELINE_COMPOSITION_HPP
#define DOCWIRE_PIPELINE_COMPOSITION_HPP

#include "chain_expression.hpp"
#include "element_base.hpp"
#include "../audit/auditor_ref.hpp"
#include "../audit/core.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace df0::pipeline
{

namespace detail
{

template<typename... ElementTypes>
struct chain_type;

template<typename ElementType>
struct chain_type<ElementType>
{
    using type = ElementType;
};

template<typename L, typename R, typename... Rest>
struct chain_type<L, R, Rest...>
    : chain_type<chain_expression<L, R>, Rest...>
{
};

template<typename... ElementTypes>
using chain_type_t = typename chain_type<ElementTypes...>::type;

} // namespace detail

/**
 * @brief A fixed compile-time composition of pipeline elements.
 *
 * This component stores the same left-associative `chain_expression`
 * graph that `operator|` builds. Its purpose is to expose composite
 * parsers as simple aliases without requiring hand-written wrapper classes.
 */
template<typename... ElementTypes>
class composition
    : public pipeline::transformer_element<composition<ElementTypes...>>
{
public:
    /**
     * @brief Constructs every composed element with the same shared arguments.
     *
     * The most common use is dependency injection with lightweight handles such
     * as `audit::auditor_ref`.
     *
     * @note Shared constructor arguments are copied into each composed element.
     *       Do not use this constructor to forward a single expiring rvalue that
     *       should be consumed only once. To move already-created processing
     *       elements into a composition, use from_elements_t or pipeline::compose(...).
     */
    template<typename... Args>
        requires (std::constructible_from<ElementTypes, Args...> && ...)
    explicit composition(Args&&... args)
        : m_chain{make_chain(std::forward<Args>(args)...)}
    {}

    /**
     * @brief Tag used to construct a composition from existing element instances.
     *
     * Unlike the universal constructor above, this constructor consumes already
     * constructed elements and wires them together with `operator|` internally.
     */
    struct from_elements_t {};

    /**
     * @brief Constructs a composition from already-created element instances.
     *
     * The elements are forwarded according to the standard pipeline ownership
     * rules:
     * - rvalue elements are moved into the composition and owned by it;
     * - lvalue elements are stored by reference and must outlive the
     *   composition.
     *
     * To transfer ownership of a named local element, pass it with std::move:
     *
     *     pipeline::compose(std::move(parser1), std::move(parser2));
     */
    template<typename... Es>
    explicit composition(from_elements_t, Es&&... elements)
        : m_chain{(... | std::forward<Es>(elements))}
    {}

    template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
    continuation operator()(message_ptr msg)
    {
        return m_chain(std::move(msg), audit::auditor_ref<stateless_auditor_type>{});
    }

    template<typename auditor_type>
    continuation operator()(
        message_ptr msg,
        audit::auditor_ref<auditor_type> auditor)
    {
        return m_chain(std::move(msg), auditor);
    }

    template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
    continuation operator()(
        message_ptr msg,
        const message_callbacks& emit_message)
    {
        return m_chain(std::move(msg), emit_message,
                       audit::auditor_ref<stateless_auditor_type>{});
    }

    template<typename auditor_type>
    continuation operator()(
        message_ptr msg,
        const message_callbacks& emit_message,
        audit::auditor_ref<auditor_type> auditor)
    {
        return m_chain(std::move(msg), emit_message, auditor);
    }

private:
    template<typename... Args>
    static auto make_chain(Args&&... args)
    {
        return (... | ElementTypes{args...});
    }

    using chain_type = detail::chain_type_t<ElementTypes...>;
    chain_type m_chain;
};

/**
 * @brief CRTP adapter that exposes a concrete composition as a pipeline element.
 *
 * This is used to create named composite pipeline elements, such as
 * `office_formats_parser` and `mail_parser`, while keeping the ergonomic
 * braced construction syntax:
 *
 *     office_formats_parser{}
 *     mail_parser{}
 *
 * @tparam Derived Concrete wrapper type, used for CRTP pipeline-role detection.
 * @tparam ElementTypes The concrete pipeline elements composing this chain.
 */
template<typename Derived, typename... ElementTypes>
class composition_wrapper
    : public pipeline::transformer_element<Derived>
{
public:
    using composition_type = composition<ElementTypes...>;

    /**
     * @brief Tests whether @p Args can construct every composed element.
     */
    template<typename... Args>
    static constexpr bool constructible_from_args =
        (std::constructible_from<ElementTypes, Args...> && ...);

    /**
     * @brief Constructs the wrapper from the same shared arguments as `composition`.
     *
     * All arguments are copied into each composed element. This is intended
     * for lightweight dependency handles such as `audit::auditor_ref`.
     */
    template<typename... Args>
        requires constructible_from_args<Args...>
    explicit composition_wrapper(Args&&... args)
        : m_composition(std::forward<Args>(args)...)
    {}

    struct from_elements_t {};

    /**
     * @brief Constructs the wrapper from already-created element instances.
     */
    template<typename... Es>
    explicit composition_wrapper(from_elements_t, Es&&... elements)
        : m_composition(
              typename composition_type::from_elements_t{},
              std::forward<Es>(elements)...)
    {}

    // Audit-aware execution forwarding.

    template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
    continuation operator()(message_ptr msg)
    {
        return m_composition(
            std::move(msg),
            audit::auditor_ref<stateless_auditor_type>{});
    }

    template<typename auditor_type>
    continuation operator()(
        message_ptr msg,
        audit::auditor_ref<auditor_type> auditor)
    {
        return m_composition(std::move(msg), auditor);
    }

    template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
    continuation operator()(
        message_ptr msg,
        const message_callbacks& emit_message)
    {
        return m_composition(
            std::move(msg),
            emit_message,
            audit::auditor_ref<stateless_auditor_type>{});
    }

    template<typename auditor_type>
    continuation operator()(
        message_ptr msg,
        const message_callbacks& emit_message,
        audit::auditor_ref<auditor_type> auditor)
    {
        return m_composition(std::move(msg), emit_message, auditor);
    }

private:
    composition_type m_composition;
};

/**
 * @brief Value-level factory for `composition`.
 *
 * This allows developers to use `pipeline::compose(parser1{}, parser2{})`
 * without spelling out the full `composition` type.
 *
 * Ownership follows `operator|` semantics:
 * - temporaries/rvalues are moved into and owned by the returned composition;
 * - lvalues are referenced and must outlive the returned composition.
 */
template<typename... ElementTypes>
auto compose(ElementTypes&&... elements)
{
    using composition_type =
        composition<std::remove_cvref_t<ElementTypes>...>;

    return composition_type{
        typename composition_type::from_elements_t{},
        std::forward<ElementTypes>(elements)...
    };
}

} // namespace df0::pipeline

#endif // DOCWIRE_PIPELINE_COMPOSITION_HPP
