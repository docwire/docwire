/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_LLAMA_RUNNER_H
#define DF0_AI_LLAMA_RUNNER_H

#include "ai_runner.h"
#include "ai_llama_export.h"
#include "model_inference_config.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0::ai::llama
{
/**
 * @brief This class is intended to load a Llama model with its correct model path and
 * respective configuration and run inference on the prompt supplied along with
 * the model configuration.
 */
template<typename auditor_type = df0::audit::null_auditor>
class DF0_AI_LLAMA_EXPORT llama_runner
    : public ai_runner,
      public with_pimpl<llama_runner<auditor_type>>,
      private audit::with_auditor<auditor_type>
{

  public:
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    llama_runner(
        const model_inference_config& config)
        : llama_runner(
              config,
              df0::audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit llama_runner(
        const model_inference_config& config,
        df0::audit::auditor_ref<auditor_type> auditor);

    std::string process(const std::string& input) override;

    std::vector<double> embed(const std::string& input) override;

    void unload() override;

  private:
    using with_pimpl<llama_runner<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
};

// Deduction guide for CTAD
llama_runner(const model_inference_config&) -> llama_runner<df0::audit::null_auditor>;

} // namespace df0::ai::llama

#endif
