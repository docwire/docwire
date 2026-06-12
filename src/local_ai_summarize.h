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

#ifndef DOCWIRE_LOCAL_AI_SUMMARIZE_H
#define DOCWIRE_LOCAL_AI_SUMMARIZE_H

#include "local_ai_export.h"
#include "ai_summarize.h"

namespace docwire::ai::local
{

class DOCWIRE_LOCAL_AI_EXPORT summarize : public docwire::ai::summarize
{
public:
    summarize(model_lifetime_policy lifetime = model_lifetime_policy::persistent);
    explicit summarize(std::shared_ptr<docwire::ai::ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_SUMMARIZE_H
