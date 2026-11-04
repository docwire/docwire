/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_CT2_FIND_H
#define DF0_LOCAL_AI_CT2_FIND_H

#include "ai_find.h"

#include "local_ai_ct2_runner_factory.h"

namespace df0::ai::local::ct2 {

class find :
	public df0::pipeline::transformer_element<find>,
	public df0::ai::find {
public:
  find(const std::string &what,
            model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::find(what, make_default_runner(), lifetime){};
};

} // namespace df0::ai::local::ct2

#endif // DF0_LOCAL_AI_CT2_FIND_H
