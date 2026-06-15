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

#ifndef DOCWIRE_AI_RUNNER_H
#define DOCWIRE_AI_RUNNER_H

#include "ai_export.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace docwire::ai {

/**
 * @brief Abstract interface for AI model runners.
 *
 * Implementations load / run / unload AI models and expose a minimal
 * synchronous API for text processing and embedding generation.
 *
 * Thread-safety requirements (MANDATORY for all derived classes):
 * - All public virtual methods (process, embed, unload) MUST be safe to
 *   call concurrently from multiple threads.
 * - Implementations must internally synchronize access to shared resources
 *   (model handles, contexts, samplers, caches, global backend state, etc.).
 * - unload() may be called concurrently with process()/embed(); implementations
 *   must either:
 *     * defer actual teardown until in-flight calls complete (reference counting,
 *       call guards, condition variables), or
 *     * make unload() idempotent and safe to call while other threads are active.
 * - The destructor MUST NOT cause undefined behavior when other threads are making
 *   calls; prefer explicit lifetime management (guards) or documented external
 *   synchronization.
 */
class DOCWIRE_AI_EXPORT ai_runner {
  public:
    /**
     * @brief Virtual destructor.
     *
     * Implementations should ensure safe destruction semantics in the presence
     * of concurrent calls (see class-level thread-safety requirements).
     */
    virtual ~ai_runner() = default;

    /**
     * @brief Synchronously process input and return generated text.
     *
     * Must be thread-safe.
     */
    virtual std::string process(const std::string& input) = 0;

    /**
     * @brief Generate an embedding for the given input.
     *
     * Must be thread-safe.
     */
    virtual std::vector<double> embed(const std::string&) = 0;
    /**
     * @brief Unload the model and free associated resources.
     * --!Must be thread-safe!-- and safe to call concurrently with process()/embed().
     */
    virtual void unload() = 0;
};

} // namespace docwire::ai

#endif
