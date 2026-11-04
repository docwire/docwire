/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

namespace df0::ai
{

summarize::summarize(std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime)
    : model_chain_element(summary_prompt, runner, lifetime)
{
}
} // namespace df0::ai
