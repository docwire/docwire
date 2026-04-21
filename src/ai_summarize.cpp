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
#include "ai_summarize.h"

namespace docwire::ai
{

summarize::summarize(std::shared_ptr<ai_runner> runner)
    : model_chain_element(summary_prompt, runner)
{
}
} // namespace docwire::ai
