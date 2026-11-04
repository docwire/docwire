/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_TASK_H
#define DF0_LOCAL_AI_TASK_H

#include "ai_task.h"

namespace df0::ai::local {

class task
    : public df0::pipeline::transformer_element<task>
    , public df0::ai::task
{
public:
  explicit task(
      const std::string &prompt, std::shared_ptr<df0::ai::ai_runner> runner,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::task(prompt, runner, lifetime){};
};

} // namespace df0::ai::local

#endif // DF0_LOCAL_AI_TASK_H
