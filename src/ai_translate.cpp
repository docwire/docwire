/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "ai_translate.h"
#include "model_chain_element.h"

namespace df0::ai
{

translate::translate(const std::string& language, std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime)
    : model_chain_element(
          "Your task is to translate every message to " + language + " language.\n\n", runner, lifetime)
{
}
} // namespace df0::ai
