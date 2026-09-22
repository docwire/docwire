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

#ifndef DOCWIRE_LOCAL_AI_FIND_H
#define DOCWIRE_LOCAL_AI_FIND_H

#include "ai_find.h"

namespace docwire::ai::local {

class find : public docwire::ai::find {
public:
  explicit find(const std::string &what,
                     std::shared_ptr<ai_runner> runner)
  : docwire::ai::find(what, runner){};
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_FIND_H
