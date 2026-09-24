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

#ifndef DOCWIRE_TRANSFORMER_FUNC_H
#define DOCWIRE_TRANSFORMER_FUNC_H

#include "chain_element.h"
#include "core_export.h"

#include <concepts>
#include <type_traits>
#include <utility>

namespace docwire
{

/**
 * @brief Wraps a callable into a static chain element.
 *
 * The callable type is preserved by value. No `std::function` is used.
 */
template <typename Func>
class transformer_func : public pipeline::transformer_element<transformer_func<Func>>
{
public:
    transformer_func(Func func)
        : m_func{std::move(func)}
    {
    }

    continuation operator()(message_ptr msg,
                            const message_callbacks& emit_message)
    {
        return m_func(std::move(msg), emit_message);
    }

private:
    Func m_func;
};

template <typename Func>
transformer_func(Func) -> transformer_func<Func>;

template <typename Chain, typename Func>
    requires chain_element_type<Chain>
          && (!chain_element_type<Func>)
          && std::invocable<std::remove_cvref_t<Func>&,
                            message_ptr,
                            const message_callbacks&>
auto operator|(Chain&& chain, Func&& func)
{
    using callable_type = std::remove_cvref_t<Func>;

    return std::forward<Chain>(chain)
         | transformer_func<callable_type>{std::forward<Func>(func)};
}

} // namespace docwire

#endif //DOCWIRE_TRANSFORMER_FUNC_H
