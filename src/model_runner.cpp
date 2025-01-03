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
#include <ctranslate2/translator.h>
#include "error_tags.h"
#include "log.h"
#include <onmt/Tokenizer.h>
#include <optional>
#include "resource_path.h"
#include "throw_if.h"

namespace docwire
{

namespace
{
    struct tokenizer_config
    {
        tokenizer_config(const std::filesystem::path& model_data_path)
        {
            try
            {
                std::ifstream ifs(model_data_path.string() + "/tokenizer_config.json");
                const auto tokenizer_config = boost::json::parse(ifs).as_object();
                tokenizer_class = tokenizer_config.at("tokenizer_class").as_string().c_str();
                if (std::filesystem::exists(model_data_path / "spiece.model"))
                    tokenizer_model_path = model_data_path / "spiece.model";
                if (tokenizer_config.contains("eos_token"))
                    eos_token = tokenizer_config.at("eos_token").as_string().c_str();
            }
            catch (const std::exception& e)
            {
                std::throw_with_nested(make_error(model_data_path));
            }
        }
        std::string tokenizer_class;
        std::filesystem::path tokenizer_model_path;
        std::optional<std::string> eos_token;
    };

    class tokenizer
    {
    public:
        tokenizer(const std::filesystem::path& model_data_path)
            : tokenizer(tokenizer_config(model_data_path))
        {}

        tokenizer(const tokenizer_config& tokenizer_config)
            : m_tokenizer_config(tokenizer_config), m_tokenizer(create_tokenizer(tokenizer_config))
        {}

        std::vector<std::string> tokenize(const std::string& input)
        {
            docwire_log_func();
            std::vector<std::string> input_tokens;
            m_tokenizer.tokenize(input.c_str(), input_tokens);
            docwire_log_var(input_tokens);
            if (m_tokenizer_config.eos_token)
                input_tokens.push_back(*m_tokenizer_config.eos_token);
            return input_tokens;
        }
        std::string detokenize(const std::vector<std::string>& output_tokens)
        {
            docwire_log_func();
            return m_tokenizer.detokenize(output_tokens).c_str();
        }
    private:
        onmt::Tokenizer create_tokenizer(const tokenizer_config& tokenizer_config)
        {
            throw_if(tokenizer_config.tokenizer_class != "T5Tokenizer",
                "Unsupported tokenizer class",
                tokenizer_config.tokenizer_class, errors::uninterpretable_data{});
            return onmt::Tokenizer(onmt::Tokenizer::Mode::None, onmt::Tokenizer::Flags::SentencePieceModel, tokenizer_config.tokenizer_model_path.string());
        }

        onmt::Tokenizer m_tokenizer;
	    tokenizer_config m_tokenizer_config;
    };

std::filesystem::path default_model_path()
{
    std::filesystem::path def_model_path = resource_path("flan-t5-large-ct2-int8");
    throw_if (!std::filesystem::exists(def_model_path),
        "Default model path does not exist", def_model_path, errors::program_corrupted{});
    return def_model_path;
}

} // anonymous namespace

template<>
struct pimpl_impl<local_ai::model_runner> : pimpl_impl_base
{
	ctranslate2::Translator m_translator;
	tokenizer m_tokenizer;

    pimpl_impl(const std::filesystem::path& model_data_path)
        : m_translator(ctranslate2::models::ModelLoader{model_data_path.string()}),
          m_tokenizer(model_data_path)
    {}

    std::vector<std::string> process(const std::vector<std::string>& input_tokens)
    {
        docwire_log_func();
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
		auto results = m_translator.translate_batch_async({ input_tokens }, options);
        throw_if (results.size() != 1, "Unexpected number of results", results.size(), errors::program_logic{});
        auto result = results[0].get();
        throw_if (result.hypotheses.size() != 1, "Unexpected number of hypotheses", result.hypotheses.size(), errors::program_logic{});
        auto hypothesis = result.hypotheses[0];
        return hypothesis;
    }
};

namespace local_ai
{

model_runner::model_runner()
    : model_runner(default_model_path())
{}

model_runner::model_runner(const std::filesystem::path& model_data_path)
    : with_pimpl(model_data_path)
{}

std::string model_runner::process(const std::string& input)
{
    std::vector<std::string> input_tokens = impl().m_tokenizer.tokenize(input);
    std::vector<std::string> output_tokens = impl().process(input_tokens);
    return impl().m_tokenizer.detokenize(output_tokens);
}

} // namespace local_ai
} // namespace docwire
