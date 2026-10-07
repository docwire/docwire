#ifndef DOCWIRE_PIPELINE_COMPOSITION_HPP
#define DOCWIRE_PIPELINE_COMPOSITION_HPP

#include "chain_expression.hpp"
#include "../audit/auditor_ref.hpp"
#include "../audit/core.hpp"
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
{
public:
    /**
     * @brief Constructs every composed element with the same arguments.
     *
     * The most common case for SDK parsers is:
     * `composition<ElementTypes...>{audit::auditor_ref<auditor_type>{auditor}}`.
     */
    template<typename... Args>
        requires (std::constructible_from<ElementTypes, Args...> && ...)
    explicit composition(Args&&... args)
        : m_chain{make_chain(std::forward<Args>(args)...)}
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
        return (... | ElementTypes{std::forward<Args>(args)...});
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
    return composition<std::remove_cvref_t<ElementTypes>...>{
        std::forward<ElementTypes>(elements)...
    };
}

} // namespace docwire::pipeline

#endif // DOCWIRE_PIPELINE_COMPOSITION_HPP
