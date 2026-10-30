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

#ifndef DOCWIRE_LOCAL_AI_CT2_RUNNER_FACTORY_H
#define DOCWIRE_LOCAL_AI_CT2_RUNNER_FACTORY_H

#include "ai_runner.h"
#include "ct2_runner.h"
#include "resource_path.h"
#include <memory>

namespace docwire::ai::local::ct2 {
inline std::shared_ptr<docwire::ai::ai_runner> make_default_runner() {
  return std::make_shared<docwire::ai::ct2::ct2_runner>(
      resource_path("flan-t5-large-ct2-int8"));
};
inline std::shared_ptr<docwire::ai::ai_runner> make_embedding_runner() {
  return std::make_shared<docwire::ai::ct2::ct2_runner>(
      resource_path("multilingual-e5-small-ct2-int8"));
};

} // namespace docwire::ai::local::ct2
#endif
