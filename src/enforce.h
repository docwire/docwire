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

#ifndef DOCWIRE_ENFORCE_H
#define DOCWIRE_ENFORCE_H

#include "safety_policy.h"
#include "with_source_location.h"
#include "make_error.h"
#include "error_tags.h"
#include "debug_assert.h"

namespace docwire
{

/**
 * @brief Enforces a condition based on the safety policy.
 * 
 * If the condition is false:
 * - In `strict` mode: Throws a `docwire::error` exception with the `docwire::errors::program_logic` tag attached and the provided context.
 * - In `relaxed` mode: Triggers a `debug_assert`, which terminates in debug builds but does nothing in release builds.
 * 
 * @tparam safety_level The safety policy to apply (default is strict).
 * @param condition The condition to check.
 * @param context Additional context information to include in the error/assertion.
 * @sa safety_policy
 * @sa debug_assert
 */
template<safety_policy safety_level = default_safety_level, typename... Context>
void enforce(detail::with_source_location<bool> condition, Context&&... context)
{
    if (!condition.value)
    {
        if constexpr (safety_level == strict) {
            auto context_tuple = std::make_tuple(std::forward<Context>(context)..., errors::program_logic{});
                throw errors::make_error_from_tuple(condition.location, std::move(context_tuple));
        } else { // relaxed
            debug_assert(condition, "Contract violation", std::forward<Context>(context)..., errors::program_logic{});
        }
    }
}

}

#endif // DOCWIRE_ENFORCE_H