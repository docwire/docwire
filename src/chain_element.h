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
 * @brief Compile-time detection of generator (source) pipeline elements.
 *
 * A generator produces the initial message of a pipeline (e.g.
 * `input_chain_element`). There are no implicit defaults: only types that
 * explicitly specialize this trait are considered generators.
 *
 * @tparam T The type to classify.
 *
 * @see is_leaf
 * @see chain_element
 */
template <typename T>
struct is_generator : std::false_type {};

/**
 * @brief Compile-time detection of leaf (terminal) pipeline elements.
 *
 * A leaf terminates a pipeline (e.g. `output_chain_element`). There are no
 * implicit defaults: only types that explicitly specialize this trait are
 * considered leaves.
 *
 * @tparam T The type to classify.
 *
 * @see is_generator
 * @see chain_element
 */
template <typename T>
struct is_leaf : std::false_type {};

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

        if constexpr (pipeline::is_generator<L>::value
                   && pipeline::is_leaf<R>::value)
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

}
#endif //DOCWIRE_CHAIN_ELEMENT_H
