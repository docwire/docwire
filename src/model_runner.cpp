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

#include "model_runner.h"

#include <boost/json.hpp>
#include <cmath>
#include <ctranslate2/encoder.h>
#include <ctranslate2/ops/mean.h>
#include <ctranslate2/translator.h>
#include "error_tags.h"
#include "log.h"
#include "throw_if.h"
#include "tokenizer.h"
#include <variant>

namespace docwire
{

namespace
{

std::variant<ctranslate2::Translator, ctranslate2::Encoder> load_model(const std::filesystem::path& model_data_path)
{
    try
    {
        docwire_log(info) << "Attempting to load model as Translator: " << model_data_path;
        return std::variant<ctranslate2::Translator, ctranslate2::Encoder>{
            std::in_place_type<ctranslate2::Translator>,
            ctranslate2::models::ModelLoader{model_data_path.string()}};
    }
    catch (const std::exception& translator_error)
    {
        docwire_log(warning) << "Failed to load model as Translator, trying as Encoder. " << translator_error;
        try
        {
            docwire_log(info) << "Attempting to load model as Encoder: " << model_data_path;
            return std::variant<ctranslate2::Translator, ctranslate2::Encoder>{
                std::in_place_type<ctranslate2::Encoder>,
                ctranslate2::models::ModelLoader{model_data_path.string()}};
        }
        catch (const std::exception& encoder_error)
        {
            std::throw_with_nested(make_error("Failed to load model as either Translator or Encoder", model_data_path, errors::program_corrupted{}));
        }
    }
}

} // anonymous namespace

template<>
struct pimpl_impl<local_ai::model_runner> : pimpl_impl_base
{
	std::variant<ctranslate2::Translator, ctranslate2::Encoder> m_model;
	local_ai::tokenizer m_tokenizer;

    pimpl_impl(const std::filesystem::path& model_data_path)
        : m_model(load_model(model_data_path)),
          m_tokenizer(model_data_path)
    {}

    std::vector<std::string> process(const std::vector<std::string>& input_tokens)
    {
        docwire_log_func();
        throw_if(!std::holds_alternative<ctranslate2::Translator>(m_model), "Model is not a Translator, cannot process.", errors::program_logic{});
        auto& translator = std::get<ctranslate2::Translator>(m_model);
        ctranslate2::TranslationOptions options{};
		options.max_decoding_length = 1024;
		options.sampling_temperature = 0.0;
		options.beam_size = 1;
        options.disable_unk = true;
		options.callback = [](ctranslate2::GenerationStepResult step_result)->bool
		{
            docwire_log_var(step_result.token);
			return false;
		};
		auto results = translator.translate_batch_async({ input_tokens }, options);
        throw_if (results.size() != 1, "Unexpected number of results", results.size(), errors::program_logic{});
        auto result = results[0].get();
        throw_if (result.hypotheses.size() != 1, "Unexpected number of hypotheses", result.hypotheses.size(), errors::program_logic{});
        auto hypothesis = result.hypotheses[0];
        return hypothesis;
    }

    std::vector<double> embed(const std::string& input)
    {
        docwire_log_func();
        throw_if(!std::holds_alternative<ctranslate2::Encoder>(m_model), "Model is not an Encoder, cannot embed.", errors::program_logic{});
        auto& encoder = std::get<ctranslate2::Encoder>(m_model);

        // 1. Tokenize
        std::vector<std::vector<std::string>> tokens_batch = { m_tokenizer.tokenize(input) };

        // 2. Forward through the encoder
        std::future<ctranslate2::EncoderForwardOutput> future = encoder.forward_batch_async(tokens_batch);
        ctranslate2::EncoderForwardOutput encoder_output = future.get();
        const ctranslate2::StorageView& last_hidden_state = encoder_output.last_hidden_state;

        // 3. Pool the result (mean pooling) using the optimized CTranslate2 operator.
        ctranslate2::StorageView pooled_result;
        ctranslate2::ops::Mean(1)(last_hidden_state, pooled_result);

        // 4. Manually perform L2 normalization.
        // This is required for sentence-transformer models like E5.
        const float* pooled_data = pooled_result.to(ctranslate2::DataType::FLOAT32).data<float>();
        // Convert to double-precision vector *before* normalization to maintain accuracy.
        std::vector<double> embedding_values(pooled_data, pooled_data + pooled_result.size());

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

namespace local_ai
{

model_runner::model_runner(const std::filesystem::path& model_data_path)
    : with_pimpl(model_data_path)
{}

std::string model_runner::process(const std::string& input)
{
    std::vector<std::string> input_tokens = impl().m_tokenizer.tokenize(input);
    std::vector<std::string> output_tokens = impl().process(input_tokens);
    return impl().m_tokenizer.detokenize(output_tokens);
}

std::vector<double> model_runner::embed(const std::string& input)
{
    return impl().embed(input);
}

} // namespace local_ai
} // namespace docwire
