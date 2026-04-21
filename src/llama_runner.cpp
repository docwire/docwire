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

#include "llama_runner.h"
#include "error_tags.h"
#include "llama_handler.h"
#include "throw_if.h"
#include <cmath>
#include <condition_variable>
#include <iostream>
#include <llama.h>
#include <mutex>

namespace docwire
{

namespace
{

std::mutex llama_backend_mutex;
std::condition_variable llama_backend_cv;
std::size_t runner_count = 0;
std::size_t active_calls = 0;
bool g_verbose = false;
/**
 * @brief Manages global lifetime of llama.cpp backend.
 *
 * llama.cpp requires explicit global initialization and teardown via:
 *   - llama_backend_init()
 *   - llama_backend_free()
 *
 * This guard implements a reference-counted lifetime model:
 *
 * - The first live llama_runner initializes the backend.
 * - The last destroyed llama_runner frees the backend.
 *
 * Thread Safety:
 * - Protected by a global mutex.
 * - Teardown waits for all active inference calls to complete.
 *
 * This prevents undefined behavior if a runner is destroyed while
 * another thread is still performing inference.
 */
struct llama_backend_guard
{
    llama_backend_guard()
    {
        std::lock_guard<std::mutex> lock(llama_backend_mutex);
        if (runner_count++ == 0) {
            llama_backend_init();
        }
    }

    ~llama_backend_guard()
    {
        std::unique_lock<std::mutex> lock(llama_backend_mutex);
        if (--runner_count == 0) {
            llama_backend_cv.wait(lock, [] { return active_calls == 0; });
            llama_backend_free();
        }
    }

    static void acquire_call()
    {
        std::lock_guard<std::mutex> lock(llama_backend_mutex);
        ++active_calls;
    }

    static void release_call()
    {
        std::lock_guard<std::mutex> lock(llama_backend_mutex);
        if (--active_calls == 0 && runner_count == 0) {
            llama_backend_cv.notify_all();
        }
    }
};
/**
 * @brief Tracks active inference calls.
 *
 * Each call to llama_runner::process() creates a llama_call_guard.
 *
 * Responsibilities:
 * - Increments the global active_calls counter on entry.
 * - Decrements it on exit.
 *
 * This ensures that backend teardown is deferred until all
 * in-flight llama_decode() calls have completed.
 *
 * Used together with llama_backend_guard to provide safe
 * concurrent inference and deterministic backend shutdown.
 */

struct llama_call_guard
{
    llama_call_guard() { llama_backend_guard::acquire_call(); }
    ~llama_call_guard() { llama_backend_guard::release_call(); }
};

} // anonymous namespace

template <> struct pimpl_impl<ai::llama::llama_runner> : pimpl_impl_base
{
    std::mutex model_mutex;
    llama_backend_guard llama_backend;
    ai::model_inference_config config;
    ai::llama::llama_handle<llama_model> model;
    ai::llama::llama_handle<llama_context> ctx;
    ai::llama::llama_handle<llama_sampler> sampler;
    const llama_vocab* vocab = nullptr;

    static void llamaLogCallback(ggml_log_level level, const char* text, void* /*user*/)
    {
        if (g_verbose || level == GGML_LOG_LEVEL_ERROR) {
            std::cerr << text;
        }
    }
    pimpl_impl(const ai::model_inference_config& cfg) : config(cfg)
    {
        g_verbose = config.verbose;
        //  Redirect llama.cpp's logs through our callback
        llama_log_set(llamaLogCallback, nullptr);
    }

    void ensure_model_loaded()
    {
        std::lock_guard<std::mutex> lock(model_mutex);
        if (model)
            return;

        llama_model_params model_params = llama_model_default_params();

        model = docwire::ai::llama::llama_handle<llama_model>(
            llama_model_load_from_file(config.model_path.c_str(), model_params));

        throw_if(!model, "Failed to load llama model.", errors::program_corrupted{});
        vocab = llama_model_get_vocab(model.get());

        llama_context_params ctx_params = llama_context_default_params();

        ctx_params.n_ctx = config.n_ctx.get();
        ctx_params.n_batch = config.n_batch.get();
        ctx_params.n_threads = config.n_threads.get();
        ctx_params.embeddings = true;

        ctx = docwire::ai::llama::llama_handle<llama_context>(llama_init_from_model(model.get(), ctx_params));

        throw_if(!ctx, "Failed to create llama context.", errors::program_corrupted{});

        llama_sampler_chain_params sp = llama_sampler_chain_default_params();

        sampler = docwire::ai::llama::llama_handle<llama_sampler>(llama_sampler_chain_init(sp));

        throw_if(!sampler, "Failed to create sampler.", errors::program_corrupted{});

        llama_sampler_chain_add(sampler.get(),
                                llama_sampler_init_min_p(config.min_probability.get(), 1));

        llama_sampler_chain_add(sampler.get(), llama_sampler_init_temp(config.temp.get()));

        llama_sampler_chain_add(sampler.get(), llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
        if (!config.grammar.empty()) {
            const llama_vocab* vocab = llama_model_get_vocab(model.get());
            llama_sampler_chain_add(sampler.get(),
                                    llama_sampler_init_grammar(vocab, config.grammar.c_str(),
                                                               config.grammar_root.c_str()));
        }
    }

    void reset()
    {
        // Get the memory handle first
        llama_memory_t mem = llama_get_memory(ctx.get());
        // Then clear all sequences
        llama_memory_seq_rm(mem, -1, -1, -1);
        llama_sampler_reset(sampler.get());
    }

    void llama_unload()
    {
        std::lock_guard<std::mutex> lock(model_mutex);
        sampler.reset();
        ctx.reset();
        model.reset();
    }

    /**
     * @brief Builds the chat-template prompt by combining
     *          system prompt from config and user prompt
     * @param user_input prompt given by user for a certain task
     */
    std::string build_prompt(const std::string& user_input) const
    {
        std::vector<llama_chat_message> messages = {{"system", config.system_prompt.c_str()},
                                                    {"user", user_input.c_str()}};

        std::string prompt;
        const char* tmpl = llama_model_chat_template(model.get(), nullptr);

        if (tmpl)
        {
            int32_t req = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, nullptr, 0);
            throw_if(req <= 0, "Template size query failed", errors::program_logic{});

            std::vector<char> formatted(req + 1, '\0');
            int32_t written = llama_chat_apply_template(tmpl, messages.data(), messages.size(),
                                                        true, formatted.data(), formatted.size());
            throw_if(written <= 0, "Template formatting failed", errors::program_logic{});

            prompt.assign(formatted.data(), written);
        }
        else
        {
        	// Fallback ChatML template
            prompt = "<|im_start|>system\n" + config.system_prompt +
                     "\n"
                     "<|im_end|>\n"
                     "<|im_start|>user\n" +
                     user_input +
                     "\n"
                     "<|im_end|>\n"
                     "<|im_start|>assistant\n";
        }
        return prompt;
    }

    /**
     * @brief Tokenizes entire prompt and return the token vector.
     * @param prompt
     */
    std::vector<llama_token> tokenize(const std::string& prompt) const
    {
        int n_tokens = llama_tokenize(vocab, prompt.c_str(), static_cast<int>(prompt.size()),
                                      nullptr, 0, false, true);
        if (n_tokens < 0)
            n_tokens = -n_tokens;

        throw_if(n_tokens == 0, "Empty tokenization result", errors::program_logic{});

        std::vector<llama_token> tokens(n_tokens);
        int written = llama_tokenize(vocab, prompt.c_str(), static_cast<int>(prompt.size()),
                                     tokens.data(), tokens.size(), false, true);

        throw_if(written != n_tokens, "Tokenization mismatch.", errors::program_logic{});
        throw_if(static_cast<size_t>(n_tokens) > config.n_ctx.get(),
                 "Input exceeds context window.", errors::program_logic{});

        return tokens;
    }


    /**
     * @brief This function feeds tokens into the context in batches
     * @param tokens
     */
    void decode_prompt(const std::vector<llama_token>& tokens)
    {
        const int32_t n_batch = static_cast<int32_t>(config.n_batch.get());
        llama_pos pos = 0;

        for (size_t start = 0; start < tokens.size(); start += n_batch) {
            int32_t len = std::min(n_batch, static_cast<int32_t>(tokens.size() - start));

            llama_batch batch =
                llama_batch_get_one(const_cast<llama_token*>(tokens.data() + start), len);

            throw_if(llama_decode(ctx.get(), batch) != 0, "Initial decode failed",
                     errors::program_logic{});

            pos += len;
        }
    }

    /**
     * @brief This function generates response from the model and returns
     */
    std::string generate()
    {
        std::string output;
        const int max_tokens = static_cast<int>(config.max_tokens.get());

        for (int i = 0; i < max_tokens; ++i) {
            llama_token token = llama_sampler_sample(sampler.get(), ctx.get(), -1);
            llama_sampler_accept(sampler.get(), token);

            if (llama_vocab_is_eog(vocab, token))
                break;

            char buf[256];
            int n = llama_token_to_piece(vocab, token, buf, sizeof(buf), 0, true);
            if (n > 0) {
                output.append(buf, n);
            }

            llama_batch batch = llama_batch_get_one(&token, 1);

            if (llama_decode(ctx.get(), batch) != 0)
                break;
        }
        return output;
    }

    /**
     * @brief This function provides abstraction for actual process function
     */
    std::string process(const std::string& user_input)
    {
        ensure_model_loaded();
        reset();

        std::string prompt = build_prompt(user_input);
        auto tokens = tokenize(prompt);

        decode_prompt(tokens);
        return generate();
    }
};

namespace ai::llama
{
llama_runner::llama_runner(const model_inference_config& config) : with_pimpl(config) {}

void llama_runner::unload() { impl().llama_unload(); }

/*
 * This function runs inference on the given model provided to Llama
 */
std::string llama_runner::process(const std::string& input)
{
    llama_call_guard guard;
    return impl().process(input);
}

/**
 * @brief Generates an embedding vector for the given input string.
 */
std::vector<double> llama_runner::embed(const std::string& input)
{
    llama_call_guard guard;
    auto& impl = this->impl();

    impl.ensure_model_loaded();
    impl.reset();

    throw_if(llama_model_n_embd(impl.model.get()) <= 0, "Model has no embedding dimension.",
             errors::program_logic{});

    const llama_vocab* vocab = llama_model_get_vocab(impl.model.get());

    int n_tokens = llama_tokenize(vocab, input.c_str(), input.length(), nullptr, 0, true, false);

    throw_if(n_tokens <= 0, "Cannot embed empty input.", errors::program_logic{});

    std::vector<llama_token> tokens(n_tokens);

    int written = llama_tokenize(vocab, input.c_str(), input.length(), tokens.data(), tokens.size(),
                                 true, false);

    throw_if(written != n_tokens, "Tokenization mismatch.", errors::program_logic{});

    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

    throw_if(llama_decode(impl.ctx.get(), batch) != 0, "Decode failed during embedding.",
             errors::program_logic{});

    const float* all_embeddings = llama_get_embeddings(impl.ctx.get());

    throw_if(!all_embeddings, "Embeddings not available from model.", errors::program_logic{});

    const int n_embd = llama_model_n_embd(impl.model.get());

    std::vector<double> result(n_embd, 0.0);

    //  Mean pooling
    for (int t = 0; t < n_tokens; ++t) {
        const float* token_emb = all_embeddings + t * n_embd;

        for (int i = 0; i < n_embd; ++i) {
            result[i] += static_cast<double>(token_emb[i]);
        }
    }

    for (double& v : result) {
        v /= static_cast<double>(n_tokens);
    }

    // L2 normalization
    double norm = 0.0;
    for (double v : result)
        norm += v * v;

    norm = std::sqrt(norm);

    if (norm > 1e-6) {
        for (double& v : result)
            v /= norm;
    }

    return result;
}

} // namespace ai::llama

} // namespace docwire
