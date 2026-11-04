/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_FIND_H
#define DF0_LOCAL_AI_FIND_H

#include "ai_find.h"

namespace df0::ai::local {

class find :
	public df0::pipeline::transformer_element<find>,
	public df0::ai::find {
public:
  explicit find(const std::string &what,
                     std::shared_ptr<ai_runner> runner)
  : df0::ai::find(what, runner){};
};

} // namespace df0::ai::local

#endif // DF0_LOCAL_AI_FIND_H
