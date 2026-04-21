/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_AI_TASK_H
#define DOCWIRE_AI_TASK_H

#include "ai_export.h"
#include "model_chain_element.h"

namespace docwire::ai
{

class DOCWIRE_AI_EXPORT task : public model_chain_element
{
  	public:
    	explicit task(const std::string& prompt, std::shared_ptr<ai_runner> runner);
};

} // namespace docwire::ai

#endif // DOCWIRE_AI_SUMMARIZE_H
