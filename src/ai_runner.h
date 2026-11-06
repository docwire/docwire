/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_AI_RUNNER_H
#define DF0_AI_RUNNER_H

#include "ai_export.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace df0::ai {

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
class DF0_AI_EXPORT ai_runner {
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
    virtual std::vector<double> embed(const std::string& input) = 0;
    /**
     * @brief Unload the model and free associated resources.
     * --!Must be thread-safe!-- and safe to call concurrently with process()/embed().
     */
    virtual void unload() = 0;
};

} // namespace df0::ai

#endif
