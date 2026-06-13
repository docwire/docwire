/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice &
 * Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text
 * extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com */
/*  Project homepage: https://github.com/docwire/docwire */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_LOCAL_AI_TRANSLATE_H
#define DOCWIRE_LOCAL_AI_TRANSLATE_H

#include "ai_translate.h"
#include "local_ai_export.h"

namespace docwire::ai::local
{

class DOCWIRE_LOCAL_AI_EXPORT translate : public docwire::ai::translate
{
  public:
 	translate(const std::string& language);
    explicit translate(const std::string& language, std::shared_ptr<ai_runner> runner);
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_TRANSLATE_H
