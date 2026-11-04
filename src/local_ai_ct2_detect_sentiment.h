/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_CT2_DETECT_SENTIMENT_H
#define DF0_LOCAL_AI_CT2_DETECT_SENTIMENT_H

#include "ai_detect_sentiment.h"
#include "local_ai_ct2_runner_factory.h"

namespace df0::ai::local::ct2 {

class detect_sentiment :
	public df0::pipeline::transformer_element<detect_sentiment>,
	public df0::ai::detect_sentiment {
public:
  detect_sentiment(model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::detect_sentiment(make_default_runner(), lifetime){};
};

} // namespace df0::ai::local::ct2

#endif // DF0_LOCAL_AI_CT2_DETECT_SENTIMENT_H
