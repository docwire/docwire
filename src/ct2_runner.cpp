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

#include "ct2_runner.h"

#include <boost/json.hpp>
#include <cmath>
#include <ctranslate2/encoder.h>
#include <ctranslate2/ops/mean.h>
#include <ctranslate2/translator.h>
#include "error_tags.h"
#include "log_entry.h"
#include "log_scope.h"
#include "serialization_exception.h" // IWYU pragma: keep
#include "serialization_filesystem.h" // IWYU pragma: keep
#include "throw_if.h"
#include "tokenizer.h"
#include <variant>

namespace docwire
{

namespace
{

std::variant<std::monostate, std::shared_ptr<ctranslate2::Translator>,
             std::shared_ptr<ctranslate2::Encoder>>
load_model(const std::filesystem::path& model_data_path)
{
    log_scope(model_data_path);
    try {
        log_scope();
        return std::make_shared<ctranslate2::Translator>(
            ctranslate2::models::ModelLoader{model_data_path.string()});
    } catch (const std::exception& translator_error) {
        log_scope(translator_error);
        try {
            log_scope();
            return std::make_shared<ctranslate2::Encoder>(
                ctranslate2::models::ModelLoader{model_data_path.string()});
        } catch (const std::exception& encoder_error) {
            std::throw_with_nested(
                make_error("Failed to load model as either Translator or Encoder", model_data_path,
                           errors::program_corrupted{}));
        }
    }
}

} // anonymous namespace

template <> struct pimpl_impl<ai::ct2::ct2_runner> : pimpl_impl_base
{
	std::mutex model_mutex;
    std::variant<std::monostate, std::shared_ptr<ctranslate2::Translator>,
                 std::shared_ptr<ctranslate2::Encoder>>
        m_model;
    ai::ct2::tokenizer m_tokenizer;
    std::filesystem::path m_model_path;

    pimpl_impl(const std::filesystem::path& model_data_path)
        : m_model_path(model_data_path), m_model(load_model(model_data_path)),
          m_tokenizer(model_data_path)
    {
    }

    std::vector<std::string> process(const std::vector<std::string>& input_tokens)
    {
        log_scope();
        std::shared_ptr<ctranslate2::Translator> translator_ptr;

        {
            std::lock_guard lock(model_mutex);
            if (std::holds_alternative<std::monostate>(m_model))
                m_model = load_model(m_model_path);

            throw_if(!std::holds_alternative<std::shared_ptr<ctranslate2::Translator>>(m_model),
                         "Model is not a Translator, cannot process.", errors::program_logic{});
            translator_ptr = std::get<std::shared_ptr<ctranslate2::Translator>>(m_model);
        }
        auto& translator = *translator_ptr;
        ctranslate2::TranslationOptions options{};
        options.max_decoding_length = 1024;
        options.sampling_temperature = 0.0;
        options.beam_size = 1;
        options.disable_unk = true;
        options.callback = [](ctranslate2::GenerationStepResult step_result) -> bool {
            log_entry(step_result.token);
            return false;
        };
        auto results = translator.translate_batch_async({input_tokens}, options);
        throw_if(results.size() != 1, "Unexpected number of results", results.size(),
                 errors::program_logic{});
        auto result = results[0].get();
        throw_if(result.hypotheses.size() != 1, "Unexpected number of hypotheses",
                 result.hypotheses.size(), errors::program_logic{});
        auto hypothesis = result.hypotheses[0];
        return hypothesis;
    }

    std::vector<double> embed(const std::string& input)
    {
        log_scope(input);
        std::shared_ptr<ctranslate2::Encoder> encoder_ptr;

        {
            std::lock_guard lock(model_mutex);
            if (std::holds_alternative<std::monostate>(m_model))
                m_model = load_model(m_model_path);
            throw_if(!std::holds_alternative<std::shared_ptr<ctranslate2::Encoder>>(m_model),
                 "Model is not an Encoder, cannot embed.", errors::program_logic{});

            encoder_ptr = std::get<std::shared_ptr<ctranslate2::Encoder>>(m_model);


        }

        auto& encoder = *encoder_ptr;

        // 1. Tokenize
        std::vector<std::vector<std::string>> tokens_batch = {m_tokenizer.tokenize(input)};

        // 2. Forward through the encoder
        std::future<ctranslate2::EncoderForwardOutput> future =
            encoder.forward_batch_async(tokens_batch);
        ctranslate2::EncoderForwardOutput encoder_output = future.get();
        ctranslate2::StorageView& last_hidden_state = encoder_output.last_hidden_state;

        // 3. Pool the result (mean pooling).
        // To correctly handle padding, we must pool only over the actual tokens.
        // The number of tokens is known before encoding, from the tokenizer output.
        // We create a new view of the hidden state that is "shrunk" to the
        // actual sequence length. This assumes a batch size of 1.
        const size_t batch_size = tokens_batch.size();
        throw_if(batch_size != 1, "Embedding function currently supports only batch size 1",
                 errors::program_logic{});
        const size_t actual_length = tokens_batch[0].size();
        const size_t hidden_size = last_hidden_state.dim(-1);

        // Create a new StorageView with the effective shape. This creates a view
        // into the existing buffer without copying data.
        // We must cast the dimensions to int64_t to avoid a narrowing conversion error,
        // as the StorageView shape constructor expects signed integers.
        ctranslate2::StorageView effective_hidden_state(
            {1, static_cast<int64_t>(actual_length), static_cast<int64_t>(hidden_size)},
            last_hidden_state.data<float>(), last_hidden_state.device());
        ctranslate2::StorageView pooled_result;
        ctranslate2::ops::Mean(1)(effective_hidden_state, pooled_result);

        // 4. Manually perform L2 normalization.
        // This is required for sentence-transformer models like E5.
        const ctranslate2::StorageView pooled_result_float =
            pooled_result.to(ctranslate2::DataType::FLOAT32);
        const float* pooled_data = pooled_result_float.data<float>();
        // Convert to double-precision vector *before* normalization to maintain accuracy.
        std::vector<double> embedding_values(pooled_data, pooled_data + pooled_result_float.size());

        double l2_norm_val = 0.0;
        for (double val : embedding_values) {
            l2_norm_val += val * val;
        }
        l2_norm_val = std::sqrt(l2_norm_val);

        // Use a small epsilon to avoid division by zero.
        // This threshold is consistent with the one in cosine_similarity.
        if (l2_norm_val > 1e-6) {
            for (double& val : embedding_values) {
                val /= l2_norm_val;
            }
        }

        return embedding_values;
    }
};

namespace ai::ct2
{

ct2_runner::ct2_runner(const std::filesystem::path& model_data_path) : with_pimpl(model_data_path)
{
}

std::string ct2_runner::process(const std::string& input)
{
    log_scope(input);
    std::vector<std::string> input_tokens = impl().m_tokenizer.tokenize(input);
    std::vector<std::string> output_tokens = impl().process(input_tokens);
    return impl().m_tokenizer.detokenize(output_tokens);
}

std::vector<double> ct2_runner::embed(const std::string& input)
{
    log_scope(input);
    return impl().embed(input);
}

void ct2_runner::unload()
{
	log_scope();
    std::lock_guard lock(impl().model_mutex);
    impl().m_model.emplace<std::monostate>();
}

} // namespace ai::ct2
} // namespace docwire
