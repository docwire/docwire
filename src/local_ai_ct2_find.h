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
