/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_EMBED_H
#define DF0_AI_EMBED_H

#include "ai_export.h"
#include "ai_runner.h"
#include "pipeline/element_base.hpp"
#include "not_null.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0::ai
{

template<typename auditor_type = audit::null_auditor>
class DF0_AI_EXPORT embed
    : public pipeline::transformer_element<embed<auditor_type>>,
      public with_pimpl<embed<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    /**
     * @brief Construct a local AI embed chain element with a specific model runner and prefix.
     *
     * @param model_runner The model runner to use for generating embeddings.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     * @param auditor Optional injected auditor for observability.
     */
    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    embed(
        not_null<std::shared_ptr<ai_runner>> model_runner,
        std::string prefix)
        : embed(std::move(model_runner), std::move(prefix),
                audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit embed(
        not_null<std::shared_ptr<ai_runner>> model_runner,
        std::string prefix,
        audit::auditor_ref<auditor_type> auditor);

    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
    using with_pimpl<embed<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
};

// Deduction guide for CTAD
embed(not_null<std::shared_ptr<ai_runner>>, std::string) -> embed<audit::null_auditor>;

} // namespace df0::ai

#endif // DF0_AI_EMBED_H
