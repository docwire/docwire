/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_PIPELINE_NOOP_TRANSFORMER_H
#define DF0_PIPELINE_NOOP_TRANSFORMER_H

#include "element_base.hpp"
#include <utility>

namespace df0::pipeline
{

/**
 * @brief A chain element that forwards every message unchanged.
 *
 * This is an explicit "no operation" intermediate step. Its primary use is as
 * an alternative inside a `variant_expression` so an optional pipeline step
 * can be represented without a separate primitive.
 *
 * @note This element is an intermediate step, modeled by `transformer_element`.
 *
 * @see variant_expression
 * @see element_base
 */
class noop_transformer : public transformer_element<noop_transformer>
{
public:
    /**
     * @brief Forwards the given message downstream unchanged.
     *
     * @param msg The message to forward.
     * @param emit_message The downstream emission callbacks.
     * @return The continuation status reported by the downstream chain.
     */
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        return emit_message(std::move(msg));
    }
};

} // namespace df0::pipeline

#endif // DF0_PIPELINE_NOOP_TRANSFORMER_H
