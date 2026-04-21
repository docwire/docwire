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

#include "ai_embed.h"
#include "local_ai_export.h"
#include <memory>

namespace docwire::ai::local
{

/**
 * @brief A chain element that generates embeddings for input text using a local AI model.
 *
 * This class is a chain element that takes a ct2_runner to generate a vector
 * embedding for a given text. It is designed to work with sentence-transformer
 * models like `multilingual-e5-small`.
 */
class DOCWIRE_LOCAL_AI_EXPORT embed :  public docwire::ai::embed
{
public:
    /**
     * @brief Construct a local AI embed chain element with a default model runner and prefix.
     *
     * This constructor initializes the embedder with a default `model_runner`
     * configured to use the `multilingual-e5-small-ct2-int8` model.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     */
    explicit embed(std::string prefix);

};

}// namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_EMBED_H
