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
#include "resource_path.h"
#include "throw_if.h"
#include "tokenizer.h"

namespace docwire
{

namespace
{

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
	local_ai::tokenizer m_tokenizer;

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
