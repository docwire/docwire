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

#include "ct2_runner.h"
#include "resource_path.h"
#include "local_ai_summarize.h"

namespace docwire::ai::local
{

summarize::summarize(model_lifetime_policy lifetime)
    : docwire::ai::summarize(
        std::make_shared<docwire::ai::ct2::ct2_runner>(resource_path("flan-t5-large-ct2-int8")), lifetime)
{}

summarize::summarize(std::shared_ptr<docwire::ai::ai_runner> runner, model_lifetime_policy lifetime)
    : docwire::ai::summarize(runner, lifetime)
{}

}
