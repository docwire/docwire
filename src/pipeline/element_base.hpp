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

#ifndef DOCWIRE_PIPELINE_ELEMENT_BASE_H
#define DOCWIRE_PIPELINE_ELEMENT_BASE_H

#include "../core_export.h"
#include "../message.h"
#include "../ref_or_owned.h"

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace docwire
{

namespace pipeline
{

struct start_processing {};

/**
 * @brief The role a pipeline element plays inside a chain.
 *
 * @see source_element
 * @see transformer_element
 * @see consumer_element
 * @see complete_pipeline
 */
enum class role
{
    source,
    transformer,
    consumer,
    complete
};

template <typename Derived>
class source_element;

template <typename Derived>
class transformer_element;

template <typename Derived>
class consumer_element;

template <typename Derived>
class complete_expression;

template <typename L, typename R>
class chain_expression;

/**
 * @brief Base class for all pipeline chain elements.
 *
 * Every source, transformer, and consumer shares this CRTP base, which grants
 * access to the most-derived type required by the static dispatch machinery.
 * A fully assembled pipeline (`complete_expression`) deliberately does NOT
 * derive from this base, so it cannot be piped any further.
 *
 * @tparam Derived The concrete pipeline element type (CRTP).
 *
 * @see element
 * @see source_element
 * @see transformer_element
 * @see consumer_element
 */
template <typename Derived>
class element_base
{
public:
    element_base() = default;
    element_base(const element_base&) = default;
    element_base& operator=(const element_base&) = default;
    element_base(element_base&&) = default;
    element_base& operator=(element_base&&) = default;
    ~element_base() = default;

    Derived& derived() noexcept
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const noexcept
    {
        return static_cast<const Derived&>(*this);
    }
};

/**
 * @brief Category base for pipeline elements that produce the initial message.
 *
 * @tparam Derived The concrete pipeline element type (CRTP).
 *
 * @see transformer_element
 * @see consumer_element
 */
template <typename Derived>
class source_element : public element_base<Derived>
{
};

/**
 * @brief Category base for pipeline elements that consume and emit messages.
 *
 * @tparam Derived The concrete pipeline element type (CRTP).
 *
 * @see source_element
 * @see consumer_element
 */
template <typename Derived>
class transformer_element : public element_base<Derived>
{
};

/**
 * @brief Category base for pipeline elements that terminate a pipeline.
 *
 * @tparam Derived The concrete pipeline element type (CRTP).
 *
 * @see source_element
 * @see transformer_element
 */
template <typename Derived>
class consumer_element : public element_base<Derived>
{
};

/**
 * @brief The terminal, executable pipeline object.
 *
 * @note This type deliberately does NOT derive from `element_base`, so a fully
 * assembled pipeline cannot be piped any further.
 *
 * @tparam Derived The concrete pipeline type (CRTP).
 */
template <typename Derived>
class complete_expression
{
};

/**
 * @brief Checks whether `T` plays the `source` role (produces the initial message).
 */
template <typename T>
concept source =
    std::derived_from<std::remove_cvref_t<T>,
                      source_element<std::remove_cvref_t<T>>>;

/**
 * @brief Checks whether `T` plays the `transformer` role (consumes and emits messages).
 */
template <typename T>
concept transformer =
    std::derived_from<std::remove_cvref_t<T>,
                      transformer_element<std::remove_cvref_t<T>>>;

/**
 * @brief Checks whether `T` plays the `consumer` role (terminates the pipeline).
 */
template <typename T>
concept consumer =
    std::derived_from<std::remove_cvref_t<T>,
                      consumer_element<std::remove_cvref_t<T>>>;

/**
 * @brief Checks whether `T` is a fully assembled, executable pipeline.
 */
template <typename T>
concept complete =
    std::derived_from<std::remove_cvref_t<T>,
                      complete_expression<std::remove_cvref_t<T>>>;

/**
 * @brief Checks whether `T` is a pipeline chain element.
 *
 * A chain element plays one of the three pipelining roles: source, transformer,
 * or consumer. A fully assembled pipeline (`complete_expression`) is
 * intentionally excluded.
 *
 * @see source
 * @see transformer
 * @see consumer
 * @see element_base
 */
template <typename T>
concept element =
    std::derived_from<std::remove_cvref_t<T>,
                      element_base<std::remove_cvref_t<T>>>;

/**
 * @brief Detects the internal chain expression node produced by `operator|`.
 *
 * @see chain
 * @see chain_expression
 */
template <typename T>
struct is_chain_expression : std::false_type {};

template <typename L, typename R>
struct is_chain_expression<chain_expression<L, R>> : std::true_type {};

/**
 * @brief Checks whether `T` is a composed pipeline chain expression.
 *
 * @see chain_expression
 */
template <typename T>
concept chain = is_chain_expression<std::remove_cvref_t<T>>::value;

/**
 * @brief Checks whether `T` may appear on the left-hand side of `operator|`.
 */
template <typename T>
concept source_or_transformer = source<T> || transformer<T>;

/**
 * @brief Checks whether `T` may appear on the right-hand side of `operator|`.
 */
template <typename T>
concept transformer_or_consumer = transformer<T> || consumer<T>;

/**
 * @brief Compile-time classification of a pipeline element's role.
 *
 * @tparam T The type to classify.
 *
 * @see role
 */
template <typename T>
constexpr role role_of()
{
    using U = std::remove_cvref_t<T>;
    static_assert(source<U> || transformer<U> || consumer<U> || complete<U>,
                  "Type does not model a pipeline element role");
    if constexpr (source<U>)
        return role::source;
    else if constexpr (transformer<U>)
        return role::transformer;
    else if constexpr (consumer<U>)
        return role::consumer;
    else
        return role::complete;
}

/**
 * @brief The pipeline role of `T` as a compile-time constant.
 */
template <typename T>
inline constexpr role role_v = role_of<T>();

} // namespace pipeline

}
#endif //DOCWIRE_PIPELINE_ELEMENT_BASE_H
