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

#include "tokenizer.h"

#include <boost/json.hpp>
#include <ctranslate2/translator.h>
#include "error_tags.h"
#include "log.h"
#include <onmt/Tokenizer.h>
#include <optional>
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

} // anonymous namespace

template<>
struct pimpl_impl<local_ai::tokenizer> : pimpl_impl_base
{
    onmt::Tokenizer m_tokenizer;
    tokenizer_config m_tokenizer_config;

    pimpl_impl(const std::filesystem::path& model_data_path)
        : pimpl_impl(tokenizer_config(model_data_path))
    {}

    pimpl_impl(const tokenizer_config& tokenizer_config)
        : m_tokenizer_config(tokenizer_config), m_tokenizer(create_tokenizer(tokenizer_config))
    {}

    onmt::Tokenizer create_tokenizer(const tokenizer_config& tokenizer_config)
    {
        throw_if(tokenizer_config.tokenizer_class != "T5Tokenizer",
            "Unsupported tokenizer class",
            tokenizer_config.tokenizer_class, errors::uninterpretable_data{});
        return onmt::Tokenizer(onmt::Tokenizer::Mode::None, onmt::Tokenizer::Flags::SentencePieceModel, tokenizer_config.tokenizer_model_path.string());
    }
};

namespace local_ai
{

tokenizer::tokenizer(const std::filesystem::path& model_data_path)
    : with_pimpl{model_data_path}
{}

std::vector<std::string> tokenizer::tokenize(const std::string& input)
{
    docwire_log_func();
    std::vector<std::string> input_tokens;
    impl().m_tokenizer.tokenize(input.c_str(), input_tokens);
    docwire_log_var(input_tokens);
    if (impl().m_tokenizer_config.eos_token)
        input_tokens.push_back(*impl().m_tokenizer_config.eos_token);
    return input_tokens;
}

std::string tokenizer::detokenize(const std::vector<std::string>& output_tokens)
{
    docwire_log_func();
    return impl().m_tokenizer.detokenize(output_tokens).c_str();
}

} // namespace local_ai
} // namespace docwire
