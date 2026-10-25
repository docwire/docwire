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
#include "local_ai_translate.h"
#include "ct2_runner.h"
#include "resource_path.h"
#include "audit/auditor_ref.hpp"
#include "audit/json_stream_auditor.hpp"

namespace docwire::ai::local
{

template<typename auditor_type>
translate<auditor_type>::translate(
    const std::string& language,
    docwire::audit::auditor_ref<auditor_type> auditor)
    : docwire::ai::translate(
          language,
          std::make_shared<docwire::ai::ct2::ct2_runner<auditor_type>>(
              resource_path("flan-t5-large-ct2-int8"),
              auditor))
{
}

template<typename auditor_type>
translate<auditor_type>::translate(
    const std::string& language,
    std::shared_ptr<docwire::ai::ai_runner> runner)
    : docwire::ai::translate(language, std::move(runner))
{
}

template class DOCWIRE_LOCAL_AI_EXPORT translate<docwire::audit::null_auditor>;
template class DOCWIRE_LOCAL_AI_EXPORT translate<docwire::audit::json_stream_auditor>;

} // namespace docwire::ai::local
