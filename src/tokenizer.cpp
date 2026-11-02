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

#include "tokenizer.h"

#include <boost/json.hpp>
#include "error_tags.h"
#include <fstream>
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include <unordered_map>
#include <optional>
#include <sentencepiece_processor.h>
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
            if (std::filesystem::exists(model_data_path / "vocabulary.json"))
                vocab_path = model_data_path / "vocabulary.json";
            else if (std::filesystem::exists(model_data_path / "shared_vocabulary.txt"))
                vocab_path = model_data_path / "shared_vocabulary.txt";
            else
                throw make_error("Could not find vocabulary.json or shared_vocabulary.txt", model_data_path, errors::program_corrupted{});
            if (tokenizer_config.contains("eos_token"))
                eos_token = tokenizer_config.at("eos_token").as_string().c_str();
            if (tokenizer_config.contains("cls_token"))
                cls_token = tokenizer_config.at("cls_token").as_string().c_str();
            if (tokenizer_config.contains("sep_token"))
                sep_token = tokenizer_config.at("sep_token").as_string().c_str();
        }
        catch (const std::exception& e)
        {
            std::throw_with_nested(make_error(model_data_path));
        }
    }
    std::string tokenizer_class;
    std::filesystem::path tokenizer_model_path;
    std::filesystem::path vocab_path;
    std::optional<std::string> eos_token;
    std::optional<std::string> cls_token;
    std::optional<std::string> sep_token;
};

} // anonymous namespace

template<typename auditor_type>
struct pimpl_impl<ai::ct2::tokenizer<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
    using audit::with_auditor<auditor_type>::auditor;

    sentencepiece::SentencePieceProcessor m_processor;
    tokenizer_config m_tokenizer_config;
    std::unordered_map<std::string, int> m_token_to_id;

    pimpl_impl(
        const std::filesystem::path& model_data_path,
        audit::auditor_ref<auditor_type> auditor)
        : pimpl_impl(tokenizer_config(model_data_path), auditor)
    {}

    pimpl_impl(
        const tokenizer_config& tokenizer_config,
        audit::auditor_ref<auditor_type> auditor)
        : audit::with_auditor<auditor_type>{auditor},
          m_tokenizer_config(tokenizer_config)
    {
        throw_if(tokenizer_config.tokenizer_class != "T5Tokenizer" &&
                     tokenizer_config.tokenizer_class != "XLMRobertaTokenizer",
                 "Unsupported tokenizer class", tokenizer_config.tokenizer_class, errors::program_corrupted{});
        throw_if(!m_processor.Load(tokenizer_config.tokenizer_model_path.string()).ok(), errors::program_corrupted{});
        load_vocabulary(tokenizer_config.vocab_path);
    }

    void load_vocabulary(const std::filesystem::path& vocab_path)
    {
        std::ifstream vocab_file(vocab_path);
        throw_if(!vocab_file.is_open(), "Failed to open vocabulary file", vocab_path);

        if (vocab_path.extension() == ".json")
        {
            // Parse JSON vocabulary from "fast" tokenizers
            // This is a flat array where the index is the token ID.
            const auto vocab_array = boost::json::parse(vocab_file).as_array();
            int id = 0;
            for (const auto& token_val : vocab_array)
            {
                m_token_to_id[token_val.as_string().c_str()] = id++;
            }
        }
        else // Assuming .txt from "slow" tokenizers
        {
            // Parse line-by-line vocabulary where the line number is the token ID.
            std::string line;
            int id = 0;
            while (std::getline(vocab_file, line))
            {
                m_token_to_id[line] = id++;
            }
        }
    }
};

namespace ai::ct2
{

template<typename auditor_type>
tokenizer<auditor_type>::tokenizer(
    const std::filesystem::path& model_data_path,
    audit::auditor_ref<auditor_type> auditor)
    : with_pimpl<tokenizer<auditor_type>>(model_data_path, auditor),
      audit::with_auditor<auditor_type>(auditor)
{}

template<typename auditor_type>
std::vector<std::string> tokenizer<auditor_type>::tokenize(const std::string& input)
{
    audit::scope audit_scope{this->auditor(), [&]{ return "input"_v = input; }};
    std::vector<std::string> input_tokens;
    throw_if(!impl().m_processor.Encode(input, &input_tokens).ok(), errors::uninterpretable_data{});
    if (impl().m_tokenizer_config.tokenizer_class == "T5Tokenizer")
    {
        throw_if(!impl().m_tokenizer_config.eos_token, errors::program_corrupted{});
        input_tokens.push_back(*impl().m_tokenizer_config.eos_token);
    }
    else if (impl().m_tokenizer_config.tokenizer_class == "XLMRobertaTokenizer")
    {
        throw_if(!impl().m_tokenizer_config.cls_token, errors::program_corrupted{});
        input_tokens.insert(input_tokens.begin(), *impl().m_tokenizer_config.cls_token);
        throw_if(!impl().m_tokenizer_config.sep_token, errors::program_corrupted{});
        input_tokens.push_back(*impl().m_tokenizer_config.sep_token);
    }
    audit::entry(this->auditor(), [&]{ return "input_tokens"_v = input_tokens; });
    return input_tokens;
}

template<typename auditor_type>
std::vector<int> tokenizer<auditor_type>::encode(const std::string& input)
{
    audit::scope audit_scope{this->auditor(), [&]{ return "input"_v = input; }};

    // 1. Get string tokens (this already includes special tokens from the tokenize method)
    const std::vector<std::string> tokens = this->tokenize(input);

    // 2. Convert string tokens to integer IDs using the loaded vocabulary map
    std::vector<int> input_ids;
    input_ids.reserve(tokens.size());
    for (const auto& token_str : tokens)
    {
        auto it = impl().m_token_to_id.find(token_str);
        throw_if(it == impl().m_token_to_id.end(), "Token not found in vocabulary", token_str, errors::uninterpretable_data{});
        input_ids.push_back(it->second);
    }

    audit::entry(this->auditor(), [&]{ return "input_ids"_v = input_ids; });
    return input_ids;
}

template<typename auditor_type>
std::string tokenizer<auditor_type>::detokenize(const std::vector<std::string>& output_tokens)
{
    audit::scope audit_scope{this->auditor(), [&]{ return "output_tokens"_v = output_tokens; }};
    std::string output;
    throw_if(!impl().m_processor.Decode(output_tokens, &output).ok(), errors::uninterpretable_data{});
    return output;
}

template class DOCWIRE_AI_CT2_EXPORT docwire::ai::ct2::tokenizer<docwire::audit::null_auditor>;
template class DOCWIRE_AI_CT2_EXPORT docwire::ai::ct2::tokenizer<docwire::audit::json_stream_auditor>;

} // namespace ai::ct2
} // namespace docwire
