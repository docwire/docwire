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

#ifndef DOCWIRE_LOCAL_AI_TOKENIZER_H
#define DOCWIRE_LOCAL_AI_TOKENIZER_H

#include "local_ai_export.h"
#include "pimpl.h"
#include <filesystem>
#include <string>
#include <vector>

namespace docwire::local_ai
{

class DOCWIRE_LOCAL_AI_EXPORT tokenizer : public with_pimpl<tokenizer>
{
public:
    tokenizer(const std::filesystem::path& model_data_path);

    std::vector<std::string> tokenize(const std::string& input);

    std::vector<int> encode(const std::string& input);

    std::string detokenize(const std::vector<std::string>& output_tokens);
};

} // namespace docwire::local_ai

#endif // DOCWIRE_LOCAL_AI_TOKENIZER_H
