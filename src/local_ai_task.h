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

#ifndef DOCWIRE_LOCAL_AI_TASK_H
#define DOCWIRE_LOCAL_AI_TASK_H

#include "local_ai_export.h"
#include "ai_task.h"

namespace docwire::ai::local
{

class DOCWIRE_LOCAL_AI_EXPORT task : public docwire::ai::task
{
public:
    explicit task(const std::string& prompt, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
    explicit task(const std::string& prompt, std::shared_ptr<docwire::ai::ai_runner> runner, model_lifetime_policy lifetime = model_lifetime_policy::persistent);
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_TASK_H
