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
} // namespace pipeline

template <typename L, typename R>
class parsing_chain;

template <typename Derived>
class chain_element
{
public:
    static constexpr bool is_generator = false;
    static constexpr bool is_leaf = false;

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

    template <typename Self, typename Other>
        requires std::same_as<std::remove_cvref_t<Self>, Derived>
              && std::derived_from<std::remove_cvref_t<Other>,
                                  chain_element<std::remove_cvref_t<Other>>>
    friend auto operator|(Self&& lhs, Other&& rhs)
    {
        using L = std::remove_cvref_t<Self>;
        using R = std::remove_cvref_t<Other>;

        parsing_chain<L, R> chain{
            ref_or_owned<L>{std::forward<Self>(lhs)},
            ref_or_owned<R>{std::forward<Other>(rhs)}
        };

        if constexpr (parsing_chain<L, R>::is_complete)
        {
            chain(std::make_shared<message<pipeline::start_processing>>(
                pipeline::start_processing{}));
        }

        return chain;
    }
};

/**
 * @brief Checks whether a type models the pipeline chain element protocol.
 *
 * A type models `chain_element_type` when it derives from
 * `chain_element<Derived>` using itself as the derived type, which is required
 * for the pipeline composition operator (`operator|`) to participate.
 *
 * @tparam T The type to test.
 *
 * @see chain_element
 * @see variant_chain_element
 * @see noop_transformer
 * @note This concept is the canonical way to constrain overloads that accept
 * arbitrary pipeline elements without knowing their concrete category.
 */
template <typename T>
concept chain_element_type =
    std::derived_from<std::remove_cvref_t<T>,
                      chain_element<std::remove_cvref_t<T>>>;

/**
 * @brief Compile-time classification of a pipeline element's category.
 *
 * This trait exposes the pipeline category (`is_generator`, `is_leaf`) of a
 * type as static `constexpr` metadata. It is the canonical, allocation-free
 * mechanism for querying the category of any pipeline element without
 * instantiating it. Because `is_generator` and `is_leaf` are pure compile-time
 * type metadata rather than an open runtime operation, a trait is the
 * appropriate customization point; callable-object idioms (e.g., `tag_invoke`)
 * are intentionally avoided here.
 *
 * The primary template defaults every category flag to `false`. Types that
 * derive from `chain_element<Derived>` are classified by the constrained
 * partial specialization below, which mirrors their own `is_generator` and
 * `is_leaf` members.
 *
 * @tparam T The type to classify.
 *
 * @note Specialize this trait to classify a user-defined pipeline element that
 * does not derive from `chain_element`.
 *
 * @see chain_element
 * @see chain_element_type
 * @see parsing_chain
 * @see variant_chain_element
 */
template <typename T>
struct pipeline_category
{
    static constexpr bool is_generator = false;
    static constexpr bool is_leaf = false;
};

template <typename T>
    requires chain_element_type<T>
struct pipeline_category<T>
{
    static constexpr bool is_generator = std::remove_cvref_t<T>::is_generator;
    static constexpr bool is_leaf = std::remove_cvref_t<T>::is_leaf;
};

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
