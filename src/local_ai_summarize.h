/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DF0_LOCAL_AI_SUMMARIZE_H
#define DF0_LOCAL_AI_SUMMARIZE_H

#include "ai_summarize.h"

namespace df0::ai::local {

class summarize
    : public df0::pipeline::transformer_element<summarize>
    , public df0::ai::summarize
{
public:
  explicit summarize(
      std::shared_ptr<df0::ai::ai_runner> runner,
      model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : df0::ai::summarize(runner, lifetime){};
};

} // namespace df0::ai::local

#endif // DF0_LOCAL_AI_SUMMARIZE_H
