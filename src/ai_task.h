/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_TASK_H
#define DF0_AI_TASK_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace df0::ai
{

class DF0_AI_EXPORT task
    : public model_chain_element,
      public pipeline::transformer_element<task>
{
  	public:
    	explicit task(const std::string& prompt, std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
};

} // namespace df0::ai

#endif // DF0_AI_TASK_H
