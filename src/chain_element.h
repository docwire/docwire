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

#ifndef DOCWIRE_CHAIN_ELEMENT_H
#define DOCWIRE_CHAIN_ELEMENT_H

#include "core_export.h"
#include "message.h"
#include "ref_or_owned.h"

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
class complete_pipeline;

} // namespace pipeline

template <typename L, typename R>
class parsing_chain;

template <typename Derived>
class chain_element
{
public:
    chain_element() = default;
    chain_element(const chain_element&) = default;
    chain_element& operator=(const chain_element&) = default;
    chain_element(chain_element&&) = default;
    chain_element& operator=(chain_element&&) = default;
    ~chain_element() = default;

    Derived& derived() noexcept
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const noexcept
    {
        return static_cast<const Derived&>(*this);
    }
};

namespace pipeline
{

/**
 * @brief Category base for pipeline elements that produce the initial message.
 *
 * @tparam Derived The concrete pipeline element type (CRTP).
 *
 * @see transformer_element
 * @see consumer_element
 */
template <typename Derived>
class source_element : public chain_element<Derived>
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
class transformer_element : public chain_element<Derived>
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
class consumer_element : public chain_element<Derived>
{
};

/**
 * @brief The terminal, executable pipeline object.
 *
 * @note This type deliberately does NOT derive from `chain_element`, so a fully
 * assembled pipeline cannot be piped any further.
 *
 * @tparam Derived The concrete pipeline type (CRTP).
 */
template <typename Derived>
class complete_pipeline
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
                      complete_pipeline<std::remove_cvref_t<T>>>;

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

/**
 * @brief Checks whether a type models the pipeline chain element protocol.
 *
 * A type models `chain_element_type` when it plays any of the three pipelining
 * roles: source, transformer, or consumer.
 *
 * @tparam T The type to test.
 *
 * @see pipeline::source
 * @see pipeline::transformer
 * @see pipeline::consumer
 */
template <typename T>
concept chain_element_type =
    pipeline::source<T> || pipeline::transformer<T> || pipeline::consumer<T>;

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
