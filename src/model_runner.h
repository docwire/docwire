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

#ifndef DOCWIRE_LOCAL_AI_MODEL_RUNNER_H
#define DOCWIRE_LOCAL_AI_MODEL_RUNNER_H

#include <filesystem>
#include <memory>
#include <string>

namespace docwire::local_ai
{

/**
 * @brief Class representing the AI model loaded to memory.
 *
 * Constructor loads model to memory and makes it ready for usage.
 * Destructor frees memory used by model.
 * It is important not to duplicate the object because memory consumption can be high.
 */
class model_runner
{
public:
    /// @brief Default constructor. Loads model to memory.
    model_runner();

    /**
     * @brief Constructor. Loads model to memory.
     * @param model_data_path Path to the folder containing model files.
     */
    model_runner(const std::filesystem::path& model_data_path);

    /// @brief Destructor. Frees memory used by model.
    ~model_runner();

    /**
     * @brief Process input text using the model.
     * @param input Text to process.
     * @return Processed text.
     */
    std::string process(const std::string& input) const;

private:
    struct implementation;
    std::unique_ptr<implementation> m_impl;
};

} // namespace docwire::local_ai

#endif // DOCWIRE_LOCAL_AI_MODEL_RUNNER_H
