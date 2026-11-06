/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_SUMMARIZE_H
#define DF0_AI_SUMMARIZE_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace df0::ai
{

class DF0_AI_EXPORT summarize
    : public model_chain_element,
      public pipeline::transformer_element<summarize>
{
  	public:
    	explicit summarize(std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
	protected:
	    static constexpr const char* summary_prompt =
	        "Your task is to summarize the text:\n\n";
};

} // namespace df0::ai

#endif // DF0_AI_SUMMARIZE_H
