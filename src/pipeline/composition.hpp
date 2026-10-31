#ifndef DOCWIRE_PIPELINE_COMPOSITION_HPP
#define DOCWIRE_PIPELINE_COMPOSITION_HPP

#include "chain_expression.hpp"
#include "element_base.hpp"
#include "../audit/auditor_ref.hpp"
#include "../audit/core.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace docwire::pipeline
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
     * @brief Constructs every composed element with the same arguments.
     *
     * The most common case for SDK parsers is:
     * `composition<ElementTypes...>{audit::auditor_ref<auditor_type>{auditor}}`.
     *
     * @note Constructor arguments are copied into each composed element.
     *       This is intended for shared dependencies such as auditor
     *       or allocator wrappers. To move already-created processing
     *       elements, use from_elements_t or pipeline::compose(...).
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
     * @brief Constructs the composition from pre-built element instances.
     *
     * The chain is built using a left-associative fold of `operator|`, matching
     * the static `chain_type_t` layout used internally.
     */
    template<typename... Es>
    explicit composition(from_elements_t, Es&&... elements)
        : m_chain{(... | std::forward<Es>(elements))}
    {}

    template<typename auditor_type = audit::null_auditor>
    continuation operator()(
        message_ptr msg,
        audit::auditor_ref<auditor_type> auditor = {})
    {
        return m_chain(std::move(msg), auditor);
    }

    template<typename auditor_type = audit::null_auditor>
    continuation operator()(
        message_ptr msg,
        const message_callbacks& emit_message,
        audit::auditor_ref<auditor_type> auditor = {})
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
 * @brief Value-level factory for `composition`.
 *
 * This allows developers to use `pipeline::compose(parser1{}, parser2{})`
 * without spelling out the full `composition` type.
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

} // namespace docwire::pipeline

#endif // DOCWIRE_PIPELINE_COMPOSITION_HPP
