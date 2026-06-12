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

#include "local_ai_task.h"
#include "ct2_runner.h"
#include "resource_path.h"

namespace docwire::ai::local
{

task::task(const std::string& prompt, model_lifetime_policy lifetime)
    : docwire::ai::task(prompt, std::make_shared<docwire::ai::ct2::ct2_runner>(
                                    resource_path("flan-t5-large-ct2-int8")), lifetime)
{
}

task::task(const std::string& prompt, std::shared_ptr<docwire::ai::ai_runner> runner, model_lifetime_policy lifetime)
    : docwire::ai::task(prompt, runner, lifetime)
{
}

} // namespace docwire::ai::local
