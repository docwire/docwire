/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_DETECT_SENTIMENT_H
#define DF0_LOCAL_AI_DETECT_SENTIMENT_H

#include "ai_detect_sentiment.h"

namespace df0::ai::local {

class detect_sentiment :
	public df0::pipeline::transformer_element<detect_sentiment>,
	public df0::ai::detect_sentiment {
public:
  explicit detect_sentiment(
      std::shared_ptr<df0::ai::ai_runner> runner,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::detect_sentiment(runner, lifetime){};
};

} // namespace df0::ai::local

#endif // DF0_LOCAL_AI_DETECT_SENTIMENT_H
