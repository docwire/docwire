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

#ifndef DOCWIRE_AI_EMBED_H
#define DOCWIRE_AI_EMBED_H

#include "ai_export.h"
#include "ai_runner.h"
#include "chain_element.h"
#include "pimpl.h"

namespace docwire::ai
{

class DOCWIRE_AI_EXPORT embed : public ChainElement, public with_pimpl<embed>
{
  public:
    /**
     * @brief Construct a local AI embed chain element with a specific model runner and prefix.
     *
     * @param ai_runner The model runner to use for generating embeddings.
     * @param prefix The string to prepend to the input text. Use an empty string for no prefix.
     */
    explicit embed(std::shared_ptr<ai_runner> model_runner, std::string prefix);
    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
    bool is_leaf() const override { return false; }

  private:
    using with_pimpl<embed>::impl;

};

} // namespace docwire::ai

#endif // DOCWIRE_AI_EMBED_H
