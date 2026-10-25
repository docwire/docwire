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
#include "ai_runner.h"

#include "model_chain_element.h"
#include "model_inference_config.h"
#ifdef DOCWIRE_CT2
#include "ct2_runner.h"
#endif
#ifdef DOCWIRE_LLAMA
#include "llama_runner.h"
#endif
#ifdef DOCWIRE_LOCAL_AI
#include "local_ai_embed.h"
#include "local_ai_task.h"
#endif
#include "ai_elements.h"
#include <boost/program_options.hpp>
#include <functional>
#include <memory>
#include <fstream>
#include "analyze_data.h"
#include "classify.h"
#include "content_type.h"
#include "csv_exporter.h"
#include "archives_parser.h"
#include "detect_sentiment.h"
#include "embed.h"


#include "extract_entities.h"
#include "extract_keywords.h"
#include "find.h"
#include "html_exporter.h"
#include "language.h"
#include "audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "ref_or_owned.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include "mail_parser.h"
#include "meta_data_exporter.h"
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "pipeline/output_element.hpp"
#include "plain_text_exporter.h"
#include "post.h"
#include "resource_path.h"
#include "standard_filter.h"
#include "summarize.h"
#include "text_to_speech.h"
#include "transcribe.h"
#include "translate_to.h"
#include "version.h"
#include "pipeline/chain_expression.hpp"
#include "pipeline/input_element.hpp"
#include "pipeline/function_transformer.hpp"
#include "pipeline/variant_expression.hpp"
#include <set>
#include <variant>

using namespace docwire;

static void readme()
{
  // warning TODO: Generate readme string literal automatically.
  std::cout <<
	"DocWire SDK: Deterministic, auditable, and secure data processing in C++20.\n"
	"On-premise by default. No compromises on computational speed.\n";
}

static void version()
{
  readme();
  std::cout << std::endl << "Version: " << VERSION << std::endl;
}

namespace docwire
{
	using magic_enum::istream_operators::operator>>;
	using magic_enum::ostream_operators::operator<<;
}

namespace docwire::openai
{
	using magic_enum::istream_operators::operator>>;
	using magic_enum::ostream_operators::operator<<;
}

using magic_enum::istream_operators::operator>>;
using magic_enum::ostream_operators::operator<<;

enum class output_type { plain_text, html, csv, metadata };
enum class embed_prefix_type { none, query, passage };
template<typename T>
std::string enum_names_str()
{
	auto names = magic_enum::enum_names<T>();
	std::string names_str;
	for (auto name: names)
	{
		if (!names_str.empty())
			names_str += '|';
		names_str += name;
	}
	return names_str;
}
#ifdef DOCWIRE_LOCAL_AI
template<typename auditor_type>
static std::shared_ptr<ai::ai_runner>
create_local_runner(
    const boost::program_options::variables_map& vm,
    const std::string& default_model,
    [[maybe_unused]] audit::auditor_ref<auditor_type> auditor)
{
    if (vm.count("local-ai-model"))
    {
        std::string model_path = vm["local-ai-model"].as<std::string>();
        if (model_path.ends_with(".gguf"))
        {
	        #ifdef DOCWIRE_LLAMA
	            ai::model_inference_config config;
	            config.model_path = model_path;
	            config.n_ctx = ai::context_size{4096};
	            config.n_threads = ai::thread_count{4};
	            return std::make_shared<ai::llama::llama_runner<auditor_type>>(
	                config,
	                audit::auditor_ref<auditor_type>(auditor));
	        #else
	            throw std::runtime_error("GGUF model support requires the local-ai-llama feature");
	        #endif
        }
        #ifdef DOCWIRE_CT2
        	return std::make_shared<ai::ct2::ct2_runner<auditor_type>>(
        		model_path,
        		audit::auditor_ref<auditor_type>(auditor));
        #else
        	throw std::runtime_error("CT2 model support requires the local-ai-ct2 feature");
        #endif
    }
    #ifdef DOCWIRE_CT2
    return std::make_shared<ai::ct2::ct2_runner<auditor_type>>(
        resource_path(default_model),
        audit::auditor_ref<auditor_type>(auditor)
    );
    #else
    	throw std::runtime_error("Default local AI model requires the local-ai-ct2 feature");
    #endif
}
#endif

namespace
{

/**
 * @brief Builds a variant of an optional pipeline step and an inert noop step.
 *
 * Returns a `std::variant` holding either the produced step (when `condition`
 * is true) or a `noop_transformer` otherwise, keeping the pipeline type stable.
 */
template <typename Factory>
auto maybe(bool condition, Factory&& factory)
{
    using T = std::decay_t<std::invoke_result_t<Factory>>;
    using V = std::variant<pipeline::noop_transformer, T>;

    if (condition)
        return V{std::invoke(std::forward<Factory>(factory))};

    return V{pipeline::noop_transformer{}};
}

/**
 * @brief Feeds the given data source into a statically typed pipeline and
 *        routes its final output to std::cout.
 */
template <typename Pipeline>
void run_pipeline(data_source&& data, Pipeline&& pipeline)
{
    pipeline::input_element{std::move(data)}
        | std::forward<Pipeline>(pipeline)
        | pipeline::output_element{std::cout};
}

} // namespace

template<typename auditor_type>
int run_with_auditor(
	const boost::program_options::variables_map& vm,
	data_source data,
	auditor_type& audit);

int main(int argc, char* argv[])
{
	bool local_processing;
	bool use_stream;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help message")
		("version", "display DocWire version")
		("input-file", po::value<std::string>()->required(), "path to file to process")
		("output_type", po::value<output_type>()->default_value(output_type::plain_text), enum_names_str<output_type>().c_str())
		("http-post", po::value<std::string>(), "url to process data via http post")
		("local-ai-prompt", po::value<std::string>(), "prompt to process text via local AI model")
		("local-ai-embed", po::value<embed_prefix_type>()->implicit_value(embed_prefix_type::none), "generate embedding of text via local AI model. Optional argument selects the prefix type: (e.g. \"passage: \" or \"query: \" or \"none: \").")
		("local-ai-model", po::value<std::string>(), "path to local AI model data (build-in default model is used if not specified)")
		("openai-chat", po::value<std::string>(), "prompt to process text and images via OpenAI")
		("openai-extract-entities", "extract entities from text and images via OpenAI")
		("openai-extract-keywords", po::value<unsigned int>(), "extract N keywords/key phrases from text and images via OpenAI")
		("openai-summarize", "summarize text and images via OpenAI")
		("openai-detect-sentiment", "detect sentiment of text and images via OpenAI")
		("openai-analyze-data", "analyze text and images for inportant insights and generate conclusions via OpenAI")
		("openai-classify", po::value<std::vector<std::string>>()->multitoken(), "classify text and images via OpenAI to one of specified categories")
		("openai-translate-to", po::value<std::string>(), "language to translate text and images to via OpenAI")
		("openai-find", po::value<std::string>(), "find phrase, object or event in text and images via OpenAI")
		("openai-text-to-speech", "convert text to speech via OpenAI")
		("openai-embed", "generate embedding of text via OpenAI")
		("openai-transcribe", "convert speech to text (transcribe) via OpenAI")
		("openai-key", po::value<std::string>()->default_value(""), "OpenAI API key")
		("openai-model", po::value<openai::model>()->default_value(openai::model::gpt_5), enum_names_str<openai::model>().c_str())
		("openai-tts-model", po::value<openai::text_to_speech_model>()->default_value(openai::text_to_speech_model::gpt_4o_mini_tts), enum_names_str<openai::text_to_speech_model>().c_str())
		("openai-embed-model", po::value<openai::embed_model>()->default_value(openai::embed_model::text_embedding_3_small), enum_names_str<openai::embed_model>().c_str())
		("openai-transcribe-model", po::value<openai::transcribe_model>()->default_value(openai::transcribe_model::gpt_4o_transcribe), enum_names_str<openai::transcribe_model>().c_str())
		("openai-voice", po::value<openai::text_to_speech_voice>()->default_value(openai::text_to_speech_voice::alloy), enum_names_str<openai::text_to_speech_voice>().c_str())
		("openai-temperature", po::value<float>(), "force specified temperature for OpenAI prompts")
		("openai-image-detail", po::value<openai::image_detail>()->default_value(openai::image_detail::automatic), enum_names_str<openai::image_detail>().c_str())
		("language", po::value<std::vector<language>>()->default_value({language::eng}, "eng"), "Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.")
		("ocr-confidence-threshold", po::value<float>()->notifier([](float val) {
			if (val < 0.0f || val > 100.0f) {
				throw po::validation_error(po::validation_error::invalid_option_value,
					"ocr-confidence-threshold", std::to_string(val));
			}
		}), "Set the OCR confidence threshold (0-100). Words with confidence below this will be excluded.")
		("local-processing", po::value<bool>(&local_processing)->default_value(true), "process documents locally including OCR")
		("use-stream", po::value<bool>(&use_stream)->default_value(false), "pass file stream to SDK instead of filename")
		("min_creation_time", po::value<unsigned int>(), "filter emails by min creation time")
		("max_creation_time", po::value<unsigned int>(), "filter emails by max creation time")
		("max_nodes_number", po::value<unsigned int>(), "filter by max number of nodes")
		("folder_name", po::value<std::string>(), "filter emails by folder name")
		("attachment_extension", po::value<std::string>(), "filter by attachment type")
		("log_file", po::value<std::string>(), "set path to log file")
		("log-filter", po::value<std::string>(), "Set a custom log filter. Filters are comma-separated and can include tags (e.g., 'audit'), function names (e.g., '@func:my_func'), and file names (e.g., '@file:*_parser.cpp'). Prepend '-' to exclude.")
		("verbose,v", "Enable verbose logging (equivalent to --log-filter='*').")
	;

	po::positional_options_description pos_desc;
	pos_desc.add("input-file", -1);

	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	if (vm.count("help"))
	{
		readme();
		std::cout << std::endl << "Usage: docwire [options] file_name" << std::endl << std::endl << desc << std::endl;
		return 0;
	}

	if (vm.count("version"))
	{
		version();
		return 0;
	}

	try
	{
		po::notify(vm);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::string file_name = vm["input-file"].as<std::string>();

	data_source data = [&]() -> data_source
	{
		if (use_stream)
			return data_source{seekable_stream_ptr{
				std::make_shared<std::ifstream>(file_name, std::ios_base::binary)}};
		return data_source{std::filesystem::path{file_name}};
	}();

#ifndef DOCWIRE_CT2
	if (vm.count("local-ai-prompt") || vm.count("local-ai-embed"))
	{
		std::cerr << "Error: Local AI features requested, but this build does not include "
		             "DOCWIRE_CT2 support.\n"
		             "Rebuild with DOCWIRE_CT2 enabled to use --local-ai-prompt or "
		             "--local-ai-embed." << std::endl;
		return 1;
	}
#endif

	const bool audit_enabled =
		vm.count("log_file") ||
		vm.count("log-filter") ||
		vm.count("verbose");

	if (audit_enabled)
	{
		docwire::ref_or_owned<std::ostream> audit_stream{std::clog};
		std::unique_ptr<std::ofstream> log_file;

		if (vm.count("log_file"))
		{
			log_file = std::make_unique<std::ofstream>(vm["log_file"].as<std::string>());
			if (!log_file->is_open())
			{
				std::cerr << "Error: Unable to open log file: " << vm["log_file"].as<std::string>() << std::endl;
				return 1;
			}
			audit_stream = docwire::ref_or_owned<std::ostream>{std::move(*log_file)};
		}

		const std::string filter_spec =
			vm.count("log-filter")
				? vm["log-filter"].as<std::string>()
				: vm.count("verbose")
					? "*"
					: "";

		docwire::audit::json_stream_auditor audit{
			filter_spec,
			std::move(audit_stream)
		};

		return run_with_auditor(vm, std::move(data), audit);
	}

	docwire::audit::null_auditor audit;
	return run_with_auditor(vm, std::move(data), audit);
}

template<typename auditor_type>
int run_with_auditor(
	const boost::program_options::variables_map& vm,
	data_source data,
	auditor_type& audit)
{
	using configured_auditor = auditor_type;

	std::string file_name = vm["input-file"].as<std::string>();
	bool local_processing = vm["local-processing"].as<bool>();

	audit::entry(audit, [&]{
		return "use_stream"_v = vm["use-stream"].as<bool>(), "file_name"_v = file_name;
	});

	auto optional_suffix =
		maybe(vm.count("http-post"), [&] {
			return http::post<configured_auditor>(
				vm["http-post"].as<std::string>(),
				"",
				http::ssl_verify_peer{true},
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-chat"), [&] {
			return openai::chat<configured_auditor>(
				vm["openai-chat"].as<std::string>(),
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-extract-entities"), [&] {
			return openai::extract_entities<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-extract-keywords"), [&] {
			return openai::extract_keywords<configured_auditor>(
				vm["openai-extract-keywords"].as<unsigned int>(),
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-summarize"), [&] {
			return openai::summarize<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-detect-sentiment"), [&] {
			return openai::detect_sentiment<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-analyze-data"), [&] {
			return openai::analyze_data<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-classify"), [&] {
			const std::vector<std::string>& categories =
				vm["openai-classify"].as<std::vector<std::string>>();
			std::set<std::string> categories_set(categories.begin(), categories.end());
			return openai::classify<configured_auditor>(
				categories_set,
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-translate-to"), [&] {
			return openai::translate_to<configured_auditor>(
				vm["openai-translate-to"].as<std::string>(),
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
#ifdef DOCWIRE_CT2
		| maybe(vm.count("local-ai-prompt"), [&] {
			auto runner = create_local_runner(
				vm,
				"flan-t5-large-ct2-int8",
				docwire::audit::auditor_ref<configured_auditor>{audit});
			return ai::local::task<configured_auditor>(
				vm["local-ai-prompt"].as<std::string>(), runner);
		})
		| maybe(vm.count("local-ai-embed"), [&] {
			const auto embed_type = vm["local-ai-embed"].as<embed_prefix_type>();
			std::string prefix;

			switch (embed_type)
			{
				case embed_prefix_type::none:
					prefix = "passage: ";
					break;
				case embed_prefix_type::query:
					prefix = "query: ";
					break;
				case embed_prefix_type::passage:
					prefix = "passage: ";
					break;
			}

			auto runner = create_local_runner(
				vm,
				"multilingual-e5-small-ct2-int8",
				docwire::audit::auditor_ref<configured_auditor>{audit});

			return docwire::ai::embed<configured_auditor>{
				runner,
				prefix,
				docwire::audit::auditor_ref<configured_auditor>{audit}
			};
		})
#endif
		| maybe(vm.count("openai-find"), [&] {
			return openai::find<configured_auditor>(
				vm["openai-find"].as<std::string>(),
				vm["openai-key"].as<std::string>(),
				vm["openai-model"].as<openai::model>(),
				vm.count("openai-temperature")
					? vm["openai-temperature"].as<float>()
					: 0.0f,
				vm["openai-image-detail"].as<openai::image_detail>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-text-to-speech"), [&] {
			return openai::text_to_speech<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-tts-model"].as<openai::text_to_speech_model>(),
				vm["openai-voice"].as<openai::text_to_speech_voice>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| maybe(vm.count("openai-embed"), [&] {
			return openai::embed<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-embed-model"].as<openai::embed_model>(),
				docwire::audit::auditor_ref<configured_auditor>{audit});
		})
		| [](message_ptr msg,
			 const message_callbacks& emit_message) -> continuation
			{
				if (msg->is<ai::embedding>())
				{
					const auto& embedding_vec = msg->get<ai::embedding>().values;
					std::string embedding_str = "[";
					for (size_t i = 0; i < embedding_vec.size(); ++i)
					{
						embedding_str += std::to_string(embedding_vec[i]);
						if (i < embedding_vec.size() - 1)
							embedding_str += ", ";
					}
					embedding_str += "]";
					return emit_message(data_source{embedding_str});
				}
				return emit_message(std::move(msg));
			}
		| [](message_ptr msg,
			 const message_callbacks& emit_message) -> continuation
			{
				if (msg->is<std::exception_ptr>())
					std::clog << "[WARNING] "
						<< errors::diagnostic_message(
							   msg->get<std::exception_ptr>())
						<< std::endl;
				return emit_message(std::move(msg));
			};

	if (vm.count("openai-transcribe"))
	{
		auto pipeline =
			content_type::detector{}
			| openai::transcribe<configured_auditor>(
				vm["openai-key"].as<std::string>(),
				vm["openai-transcribe-model"].as<openai::transcribe_model>(),
				docwire::audit::auditor_ref<configured_auditor>{audit})
			| plain_text_exporter{}
			| std::move(optional_suffix);

		try
		{
			run_pipeline(std::move(data), pipeline);
		}
		catch (const std::exception& e)
		{
			std::cerr << "[ERROR] " << errors::diagnostic_message(e)
				<< "processing file " + file_name << std::endl;
			return 2;
		}
		catch (...)
		{
			std::cerr << "[ERROR] Unknown error\nprocessing file " + file_name << std::endl;
			return 2;
		}
		return 0;
	}

	if (local_processing)
	{
		ocr_confidence_threshold threshold_arg{};
		if (vm.count("ocr-confidence-threshold"))
			threshold_arg.v = vm["ocr-confidence-threshold"].as<float>();

		using exporter_variant =
			std::variant<plain_text_exporter,
			             html_exporter<configured_auditor>,
			             csv_exporter,
			             metadata_exporter<configured_auditor>>;

		exporter_variant formatter = plain_text_exporter{};

		switch (vm["output_type"].as<output_type>())
		{
			case output_type::plain_text:
				formatter = plain_text_exporter{};
				break;
			case output_type::html:
				formatter = html_exporter<configured_auditor>{
					docwire::audit::auditor_ref<configured_auditor>{audit}
				};
				break;
			case output_type::csv:
				formatter = csv_exporter{};
				break;
			case output_type::metadata:
				formatter = metadata_exporter<configured_auditor>{
					docwire::audit::auditor_ref<configured_auditor>{audit}
				};
				break;
		}

		auto local_prefix =
			content_type::detector{}
			| archives_parser<configured_auditor>{
				docwire::audit::auditor_ref<configured_auditor>{audit}}
			| office_formats_parser<docwire::safety_policy::strict, configured_auditor>{
				docwire::audit::auditor_ref<configured_auditor>{audit}}
			| mail_parser<configured_auditor>{
				docwire::audit::auditor_ref<configured_auditor>{audit}}
			| ocr_parser<configured_auditor>{
				vm["language"].as<std::vector<language>>(),
				threshold_arg,
				{},
				{},
				docwire::audit::auditor_ref<configured_auditor>{audit}}
			| maybe(vm.count("max_nodes_number"), [&] {
				  return standard_filter::filterByMaxNodeNumber(
					  vm["max_nodes_number"].as<unsigned int>());
			  })
			| maybe(vm.count("min_creation_time"), [&] {
				  return standard_filter::filterByMailMinCreationTime(
					  vm["min_creation_time"].as<unsigned int>());
			  })
			| maybe(vm.count("max_creation_time"), [&] {
				  return standard_filter::filterByMailMaxCreationTime(
					  vm["max_creation_time"].as<unsigned int>());
			  })
			| maybe(vm.count("folder_name"), [&] {
				  return standard_filter::filterByFolderName(
					  {vm["folder_name"].as<std::string>()});
			  })
			| maybe(vm.count("attachment_extension"), [&] {
				  return standard_filter::filterByAttachmentType(
					  {file_extension{
						  vm["attachment_extension"].as<std::string>()}});
			  });

		auto pipeline =
			std::move(local_prefix)
			| std::move(formatter)
			| std::move(optional_suffix);

		try
		{
			run_pipeline(std::move(data), pipeline);
		}
		catch (const std::exception& e)
		{
			std::cerr << "[ERROR] " << errors::diagnostic_message(e)
				<< "processing file " + file_name << std::endl;
			return 2;
		}
		catch (...)
		{
			std::cerr << "[ERROR] Unknown error\nprocessing file " + file_name << std::endl;
			return 2;
		}
		return 0;
	}

	auto pipeline =
		content_type::detector{}
		| std::move(optional_suffix);

	try
	{
		run_pipeline(std::move(data), pipeline);
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERROR] " << errors::diagnostic_message(e)
			<< "processing file " + file_name << std::endl;
		return 2;
	}
	catch (...)
	{
		std::cerr << "[ERROR] Unknown error\nprocessing file " + file_name << std::endl;
		return 2;
	}

	return 0;
}
