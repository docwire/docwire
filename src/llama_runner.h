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

#ifndef DOCWIRE_AI_LLAMA_RUNNER_H
#define DOCWIRE_AI_LLAMA_RUNNER_H

#include "ai_runner.h"
#include "ai_llama_export.h"
#include "model_inference_config.h"
#include "pimpl.h"

namespace docwire::ai::llama
{
/**
 * @brief This class is intended to load a Llama model with its correct model path and
 * respective configuration and run inference on the prompt supplied along with
 * the model configuration.
 */
class DOCWIRE_AI_LLAMA_EXPORT llama_runner : public ai_runner, public with_pimpl<llama_runner>
{

  public:
    explicit llama_runner(const model_inference_config& config);

    std::string process(const std::string& input) override;

    std::vector<double> embed(const std::string&) override;

    virtual void unload() override;
};

} // namespace docwire::ai::llama

#endif
