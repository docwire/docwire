/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "ai_detect_sentiment.h"

namespace df0::ai
{

detect_sentiment::detect_sentiment(std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime)
    : model_chain_element(detect_sentiment_prompt, runner, lifetime)
{
}
} // namespace df0::ai
