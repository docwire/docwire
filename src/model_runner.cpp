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

#include "exception.h"
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/json.hpp>
#include <ctranslate2/translator.h>
#include "log.h"
#include "misc.h"
#include <onmt/Tokenizer.h>
#include <optional>

namespace docwire::local_ai
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
                throw RuntimeError{"Failed to load and parse tokenizer_config.json: " + std::string{e.what()}};
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
		    if (tokenizer_config.tokenizer_class != "T5Tokenizer")
                throw std::runtime_error("Unsupported tokenizer class: " + tokenizer_config.tokenizer_class);
            return onmt::Tokenizer(onmt::Tokenizer::Mode::None, onmt::Tokenizer::Flags::SentencePieceModel, tokenizer_config.tokenizer_model_path.string());
        }

        onmt::Tokenizer m_tokenizer;
	    tokenizer_config m_tokenizer_config;
    };

inline std::filesystem::path this_line_location()
{
    return boost::dll::this_line_location().lexically_normal().string();
}

inline std::filesystem::path resource_path(const std::filesystem::path& resource_rel_path)
{
    auto path = this_line_location().parent_path();
    if (path.parent_path().filename() == "debug") {
        // If we are in a vcpkg debug build, adjust the path to access resources in the release directory
        path = path.parent_path().parent_path() / "share" / resource_rel_path;
    } else {
        // For release builds, the resources are directly in the ../share directory
        path = path.parent_path() / "share" / resource_rel_path;
    }
    if (std::filesystem::exists(path.string() + ".path"))
    {
        // Read path from path file
        std::ifstream ifs(path.string() + ".path");
        std::string redirected_path;
        std::getline(ifs, redirected_path);
        path = redirected_path;
    }
    return path;
}
} // anonymous namespace

struct model_runner::implementation
{
	ctranslate2::Translator m_translator;
	tokenizer m_tokenizer;

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
        if (results.size() != 1)
            throw LogicError("Unexpected number of results");
        auto result = results[0].get();
        if (result.hypotheses.size() != 1)
            throw LogicError("Unexpected number of hypotheses");
        auto hypothesis = result.hypotheses[0];
        return hypothesis;
    }
};

model_runner::model_runner()
    : model_runner(resource_path("flan-t5-large-ct2-int8"))
{}

model_runner::model_runner(const std::filesystem::path& model_data_path)
    : m_impl{std::make_unique<implementation>(
        ctranslate2::models::ModelLoader{model_data_path},
		tokenizer{model_data_path})}
{}

model_runner::~model_runner()
{}

std::string model_runner::process(const std::string& input) const
{
    std::vector<std::string> input_tokens = m_impl->m_tokenizer.tokenize(input);
    std::vector<std::string> output_tokens = m_impl->process(input_tokens);
    return m_impl->m_tokenizer.detokenize(output_tokens);
}

} // namespace docwire::local_ai
