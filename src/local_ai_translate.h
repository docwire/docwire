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

#ifndef DOCWIRE_LOCAL_AI_TRANSLATE_H
#define DOCWIRE_LOCAL_AI_TRANSLATE_H

#include "ai_translate.h"
#include "local_ai_export.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"

namespace docwire::ai::local
{

template<typename auditor_type = docwire::audit::null_auditor>
class DOCWIRE_LOCAL_AI_EXPORT translate
    : public docwire::pipeline::transformer_element<translate<auditor_type>>
    , public docwire::ai::translate
{
public:
    explicit translate(
        const std::string& language,
        docwire::audit::auditor_ref<auditor_type> auditor = {});

    explicit translate(
        const std::string& language,
        std::shared_ptr<docwire::ai::ai_runner> runner);
};

translate(const std::string&) -> translate<docwire::audit::null_auditor>;
translate(const std::string&, std::shared_ptr<docwire::ai::ai_runner>)
    -> translate<docwire::audit::null_auditor>;

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_TRANSLATE_H
