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

#ifndef DOCWIRE_LOCAL_AI_CT2_TRANSLATE_H
#define DOCWIRE_LOCAL_AI_CT2_TRANSLATE_H

#include "ai_translate.h"

#include "local_ai_ct2_runner_factory.h"

namespace docwire::ai::local::ct2 {

class translate : public docwire::ai::translate {
public:
  translate(const std::string &language,
            model_lifetime_policy lifetime = model_lifetime_policy::persistent)
      : docwire::ai::translate(language, make_default_runner(), lifetime){};
};

} // namespace docwire::ai::local::ct2

#endif // DOCWIRE_LOCAL_AI_CT2_TRANSLATE_H
