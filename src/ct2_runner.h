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

#ifndef DOCWIRE_AI_CT2_RUNNER_H
#define DOCWIRE_AI_CT2_RUNNER_H

#include "ai_ct2_export.h"
#include "pimpl.h"
#include <filesystem>
#include <vector>
#include <string>
#include "ai_runner.h"

namespace docwire::ai::ct2
{

/**
 * @brief Class representing the C2Translate AI model loaded to memory.
 *
 * Constructor loads model to memory and makes it ready for usage.
 * Destructor frees memory used by model.
 * It is important not to duplicate the object because memory consumption can be high.
 */
class DOCWIRE_AI_CT2_EXPORT ct2_runner :  public ai_runner, public with_pimpl<ct2_runner>
{
public:
    /**
     * @brief Constructor. Loads model to memory.
     * @param model_data_path Path to the folder containing model files.
     */
    ct2_runner(const std::filesystem::path& model_data_path);

    /**
     * @brief Process input text using the model.
     * @param input Text to process.
     * @return Processed text.
     */
    std::string process(const std::string& input) override;

    /**
     * @brief Create embedding for the input text using the model.
     * @param input Text to process.
     * @return Vector of embedding values.
     */
    std::vector<double> embed(const std::string& input) override;

    /**
     * @brief Unload the model and free associated resources.
     * --!Must be thread-safe!-- and safe to call concurrently with process()/embed().
     */
    virtual void unload() override;
};

} // namespace docwire::ai::ct2

#endif // DOCWIRE_AI_CT2_RUNNER_H
