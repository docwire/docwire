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

#ifndef DF0_AI_TRANSLATE_H
#define DF0_AI_TRANSLATE_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace df0::ai
{

class DF0_AI_EXPORT translate
    : public model_chain_element,
      public pipeline::transformer_element<translate>
{
  	public:
    	explicit translate(const std::string& language, std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
};

} // namespace df0::ai

#endif // DF0_AI_TRANSLATE_H
