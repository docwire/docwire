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

#ifndef DOCWIRE_PIPELINE_NOOP_TRANSFORMER_H
#define DOCWIRE_PIPELINE_NOOP_TRANSFORMER_H

#include "element_base.hpp"
#include <utility>

namespace docwire::pipeline
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

} // namespace docwire::pipeline

#endif // DOCWIRE_PIPELINE_NOOP_TRANSFORMER_H
