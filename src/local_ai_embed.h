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

#ifndef DOCWIRE_LOCAL_AI_EMBED_H
#define DOCWIRE_LOCAL_AI_EMBED_H

#include "chain_element.h"
#include "local_ai_export.h"
#include "model_runner.h"
#include "pimpl.h"
#include <memory>

namespace docwire::local_ai
{

/**
 * @brief A chain element that generates embeddings for input text using a local AI model.
 *
 * This class is a chain element that takes a model_runner to generate a vector
 * embedding for a given text. It is designed to work with sentence-transformer
 * models like `multilingual-e5-small`.
 */
class DOCWIRE_LOCAL_AI_EXPORT embed : public ChainElement, public with_pimpl<embed>
{
public:
    /// Common prefix for passage embeddings with E5 models.
    static const std::string e5_passage_prefix;
    /// Common prefix for query embeddings with E5 models.
    static const std::string e5_query_prefix;

    /**
     * @brief Construct a local AI embed chain element with a specific model runner and prefix.
     *
     * @param model_runner The model runner to use for generating embeddings.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     */
    explicit embed(std::shared_ptr<model_runner> model_runner, std::string prefix);

    /**
     * @brief Construct a local AI embed chain element with a default model runner and prefix.
     *
     * This constructor initializes the embedder with a default `model_runner`
     * configured to use the `multilingual-e5-small-ct2-int8` model.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     */
    explicit embed(std::string prefix);

    continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;

    bool is_leaf() const override { return false; }

private:
    using with_pimpl<embed>::impl;
};

} // namespace docwire::local_ai

#endif // DOCWIRE_LOCAL_AI_EMBED_H
