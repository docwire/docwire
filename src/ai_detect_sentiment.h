/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_DETECT_SENTIMENT_H
#define DF0_AI_DETECT_SENTIMENT_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace df0::ai {

class DF0_AI_EXPORT detect_sentiment : public model_chain_element {
public:
  explicit detect_sentiment(
      std::shared_ptr<ai_runner> runner,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent);

protected:
  static constexpr const char *detect_sentiment_prompt =
      "Detect sentiment and "
      "answer with exact sentiment category: positive, "
      "negative, mixed::\n\n";
};

} // namespace df0::ai

#endif // DF0_AI_DETECT_SENTIMENT_H
