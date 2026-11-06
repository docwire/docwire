/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_MODEL_CHAIN_ELEMENT_H
#define DF0_AI_MODEL_CHAIN_ELEMENT_H

#include "ai_runner.h"
#include "pipeline/element_base.hpp"
#include "ai_export.h"

namespace df0::ai
{

/***
 * @brief Model usage option for Load/Unload in memory
 */
enum class model_lifetime_policy
{
    persistent, // keeps the model in memory, which makes it available for next pipeline usage
    unload_after_use // unloads after current usage
};

/**
 * @brief A model chain element that processes input text using a model runner.
 *
 * This class is a chain element that takes a prompt and a model runner. It
 * processes the input by appending the prompt to the input text and then
 * passing the text to the model runner. The output of the model runner is
 * then emitted as a new message_ptr object.
 */
class DF0_AI_EXPORT model_chain_element
{
  public:
    /**
     * @brief Construct a model chain element.
     *
     * @param prompt The prompt to append to the input text.
     * @param ai_runner The model runner to use for processing the text.
     * @param model_lifetime_policy Option to decide whether to unload model after usage or keep it persistent
     */
    model_chain_element(const std::string& prompt, std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);

    /**
     * @brief Construct a model chain element with a default model runner.
     *
     * This constructor initializes the model chain element with a default
     * `model_runner` configured to use the `flan-t5-large-ct2-int8` model.
     *
     * @param prompt The prompt to append to the input text.
     * @param model_lifetime_policy Option to decide whether to unload model after usage or keep it persistent
     */
    //model_chain_element(const std::string& prompt, model_lifetime_policy lifetime = model_lifetime_policy::persistent);

    /**
     * @brief Process the input.
     *
     * If the input is not a data source, emit the input and return. If the
     * input is a data source, append the prompt to the input text and then
     * pass the text to the model runner. The output of the model runner is
     * then emitted as a new message_ptr object.
     *
     * @param msg The input message to process.
     * @param emit_message Callback used to emit derived messages downstream.
     */
    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

  private:
    std::string m_prompt;
    std::shared_ptr<ai_runner> m_model_runner;
    model_lifetime_policy m_model_lifetime;
};

} // namespace df0::ai

#endif // DF0_AI_MODEL_CHAIN_ELEMENT_H
