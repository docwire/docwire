/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "model_chain_element.h"
#include "data_source.h"
#include "error_tags.h"
#include "resource_path.h"
#include "throw_if.h"

namespace df0::ai
{
/**
 * @brief constructor to run llama models
 */
model_chain_element::model_chain_element(const std::string& prompt,
                                         std::shared_ptr<ai_runner> runner,
                                         model_lifetime_policy lifetime)
    : m_prompt(prompt), m_model_runner(std::move(runner)), m_model_lifetime(lifetime)
{
}

continuation model_chain_element::operator()(message_ptr msg, const message_callbacks& emit_message)
{
    if (!msg->is<data_source>())
        return emit_message(std::move(msg));

    const data_source& data = msg->get<data_source>();
    throw_if(!data.has_highest_confidence_mime_type_in({mime_type{"text/plain"}}),
             errors::program_logic{});
    std::string input = m_prompt + "\n" + data.string();
    std::string output = m_model_runner->process(input);
    if (m_model_lifetime == model_lifetime_policy::unload_after_use) {
        m_model_runner->unload();
    }
    return emit_message(data_source{std::move(output)});
}

} // namespace df0::ai
