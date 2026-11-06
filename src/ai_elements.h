/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_ELEMENTS_H
#define DF0_AI_ELEMENTS_H

#include "ai_export.h"
#include <vector>

namespace df0::ai
{

struct DF0_AI_EXPORT embedding
{
  std::vector<double> values;
};

} // namespace df0::ai

#endif // DF0_AI_ELEMENTS_H