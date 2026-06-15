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

#ifndef DOCWIRE_AI_SUMMARIZE_H
#define DOCWIRE_AI_SUMMARIZE_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace docwire::ai
{

class DOCWIRE_AI_EXPORT summarize : public model_chain_element
{
  	public:
    	explicit summarize(std::shared_ptr<ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
	protected:
	    static constexpr const char* summary_prompt =
	        "Your task is to summarize the text:\n\n";
};

} // namespace docwire::ai

#endif // DOCWIRE_AI_SUMMARIZE_H
