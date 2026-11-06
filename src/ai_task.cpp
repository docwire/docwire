/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "ai_task.h"

namespace df0::ai
{

task::task(const std::string& prompt, std::shared_ptr<ai_runner> runner,model_lifetime_policy lifetime)
    : model_chain_element(prompt, runner, lifetime)
{
}
} // namespace df0::ai
