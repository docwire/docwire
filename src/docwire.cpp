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
#ifdef DOCWIRE_LOCAL_CT2
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
#include "log_json_stream_sink.h"
#include "log_core.h"
#include "log_entry.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include "mail_parser.h"
#include "meta_data_exporter.h"
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
#include "resource_path.h"
#include "standard_filter.h"
#include "summarize.h"
#include "text_to_speech.h"
#include "transcribe.h"
#include "translate_to.h"
#include "version.h"
#include "parsing_chain.h"
#include "input.h"
#include <set>

using namespace docwire;

static void readme()
{
  // warning TODO: Generate readme string literal automatically.
  std::cout <<
	"/*********************************************************************************************************************************************/\n"
	"/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */\n"
	"/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */\n"
	"/*  document analysis. Offline processing possible for security and confidentiality                                                          */\n"
	"/*                                                                                                                                           */\n"
	"/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */\n"
	"/*  Project homepage: https://github.com/docwire/docwire                                                                                     */\n"
	"/*                                                                                                                                           */\n"
	"/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */\n"
	"/*********************************************************************************************************************************************/\n"
          ;
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

enum class OutputType { plain_text, html, csv, metadata };
enum class EmbedPrefixType { none, query, passage };
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
static std::shared_ptr<ai::ai_runner>
create_local_runner(const boost::program_options::variables_map& vm,
                    const std::string& default_model)
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
	            return std::make_shared<ai::llama::llama_runner>(config);
	        #else
	            throw std::runtime_error("GGUF model support requires the llama-engine feature");
	        #endif
        }
        #ifdef DOCWIRE_LOCAL_CT2
        	return std::make_shared<ai::ct2::ct2_runner>(model_path);
        #else
        	throw std::runtime_error("CT2 model support requires the ct2-engine feature");
        #endif
    }
    #ifdef DOCWIRE_LOCAL_CT2
    return std::make_shared<ai::ct2::ct2_runner>(
        resource_path(default_model)
    );
    #else
    	throw std::runtime_error("Default local AI model requires the ct2-engine feature");
    #endif
}
#endif

int main(int argc, char* argv[])
{
	// Set the default sink to std::clog.
	log::set_sink(log::json_stream_sink(std::clog));

	bool local_processing;
	bool use_stream;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help message")
		("version", "display DocWire version")
		("input-file", po::value<std::string>()->required(), "path to file to process")
		("output_type", po::value<OutputType>()->default_value(OutputType::plain_text), enum_names_str<OutputType>().c_str())
		("http-post", po::value<std::string>(), "url to process data via http post")
		("local-ai-prompt", po::value<std::string>(), "prompt to process text via local AI model")
		("local-ai-embed", po::value<EmbedPrefixType>()->implicit_value(EmbedPrefixType::none), "generate embedding of text via local AI model. Optional argument selects the prefix type: (e.g. \"passage: \" or \"query: \" or \"none: \").")
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
		("openai-model", po::value<openai::Model>()->default_value(openai::Model::gpt_5), enum_names_str<openai::Model>().c_str())
		("openai-tts-model", po::value<openai::text_to_speech::Model>()->default_value(openai::text_to_speech::Model::gpt_4o_mini_tts), enum_names_str<openai::text_to_speech::Model>().c_str())
		("openai-embed-model", po::value<openai::embed::model>()->default_value(openai::embed::model::text_embedding_3_small), enum_names_str<openai::embed::model>().c_str())
		("openai-transcribe-model", po::value<openai::transcribe::Model>()->default_value(openai::transcribe::Model::gpt_4o_transcribe), enum_names_str<openai::transcribe::Model>().c_str())
		("openai-voice", po::value<openai::text_to_speech::Voice>()->default_value(openai::text_to_speech::Voice::alloy), enum_names_str<openai::text_to_speech::Voice>().c_str())
		("openai-temperature", po::value<float>(), "force specified temperature for OpenAI prompts")
		("openai-image-detail", po::value<openai::ImageDetail>()->default_value(openai::ImageDetail::automatic), enum_names_str<openai::ImageDetail>().c_str())
		("language", po::value<std::vector<Language>>()->default_value({Language::eng}, "eng"), "Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.")
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

	if (vm.count("verbose"))
	{
		log::set_filter("*");
	}
	if (vm.count("log-filter"))
	{
		log::set_filter(vm["log-filter"].as<std::string>());
	}

	if (vm.count("log_file"))
	{
		std::ofstream log_file(vm["log_file"].as<std::string>());
		if (!log_file.is_open())
		{
			std::cerr << "Error: Unable to open log file: " << vm["log_file"].as<std::string>() << std::endl;
			return 1;
		}
		log::set_sink(log::json_stream_sink(std::move(log_file)));
	}

	std::string file_name = vm["input-file"].as<std::string>();

	log_entry(use_stream, file_name);
	auto chain = use_stream ?
		(std::ifstream{file_name, std::ios_base::binary} | content_type::detector{}) :
		(std::filesystem::path{file_name} | content_type::detector{});

	if (vm.count("openai-transcribe"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::transcribe::Model model = vm["openai-transcribe-model"].as<openai::transcribe::Model>();
		chain |= openai::transcribe(api_key, model) | plain_text_exporter();
	}
	else if (local_processing)
	{
		ocr_confidence_threshold threshold_arg{};
		if (vm.count("ocr-confidence-threshold"))
		{
			threshold_arg.v = vm["ocr-confidence-threshold"].as<float>();
		}
		chain |=
			archives_parser{} |
			office_formats_parser{} | mail_parser{} | ocr_parser{vm["language"].as<std::vector<Language>>(), threshold_arg};
		if (vm.count("max_nodes_number"))
		{
			chain |= standard_filter::filterByMaxNodeNumber(vm["max_nodes_number"].as<unsigned int>());
		}
		if (vm.count("min_creation_time"))
		{
			chain |= standard_filter::filterByMailMinCreationTime(vm["min_creation_time"].as<unsigned int>());
		}
		if (vm.count("max_creation_time"))
		{
			chain |= standard_filter::filterByMailMaxCreationTime(vm["max_creation_time"].as<unsigned int>());
		}
		if (vm.count("folder_name"))
		{
			chain |= standard_filter::filterByFolderName({vm["folder_name"].as<std::string>()});
		}
		if (vm.count("attachment_extension"))
		{
			chain |= standard_filter::filterByAttachmentType({file_extension{vm["attachment_extension"].as<std::string>()}});
		}

		switch (vm["output_type"].as<OutputType>())
		{
			case OutputType::plain_text:
				chain |= plain_text_exporter();
				break;
			case OutputType::html:
				chain |= html_exporter();
				break;
			case OutputType::csv:
				chain |= csv_exporter();
				break;
			case OutputType::metadata:
				chain |= metadata_exporter();
				break;
		}
	}

	if (vm.count("http-post"))
	{
		chain |= http::post(vm["http-post"].as<std::string>());
	}

	if (vm.count("openai-chat"))
	{
		std::string prompt = vm["openai-chat"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::chat(prompt, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-extract-entities"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::extract_entities(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-extract-keywords"))
	{
		unsigned int max_keywords = vm["openai-extract-keywords"].as<unsigned int>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::extract_keywords(max_keywords, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-summarize"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::summarize(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-detect-sentiment"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::detect_sentiment(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-analyze-data"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::analyze_data(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-classify"))
	{
		const std::vector<std::string>& categories = vm["openai-classify"].as<std::vector<std::string>>();
		std::set<std::string> categories_set(categories.begin(), categories.end());
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::classify(categories_set, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-translate-to"))
	{
		std::string language = vm["openai-translate-to"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::translate_to(language, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}
	#ifdef DOCWIRE_LOCAL_CT2
	if (vm.count("local-ai-prompt"))
	{
		try
		{
			std::string prompt = vm["local-ai-prompt"].as<std::string>();

			auto runner = create_local_runner(vm, "flan-t5-large-ct2-int8");
			chain |=
				ai::local::task(prompt, runner);
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << errors::diagnostic_message(e) << std::endl;
			return 1;
		}
	}

	if (vm.count("local-ai-embed"))
	{
		try
		{
			EmbedPrefixType prefix_type = vm["local-ai-embed"].as<EmbedPrefixType>();
			if (prefix_type == EmbedPrefixType::query)
			{
				chain |= ai::local::query::embedder();
			} else if (prefix_type == EmbedPrefixType::passage) {
          		chain |= ai::local::passage::embedder();
	        } else {
           		chain |= ai::local::passage::embedder();
           	}
			chain |= [](message_ptr msg, const message_callbacks& emit_message) -> continuation {
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
			};
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << errors::diagnostic_message(e) << std::endl;
			return 1;
		}
	}
	#else
	if (vm.count("local-ai-prompt") || vm.count("local-ai-embed"))
	{
		std::cerr << "Error: Local AI features requested, but this build does not include "
		             "DOCWIRE_LOCAL_CT2 support.\n"
		             "Rebuild with DOCWIRE_LOCAL_CT2 enabled to use --local-ai-prompt or "
		             "--local-ai-embed." << std::endl;
		return 1;
	}
	#endif

	if (vm.count("openai-find"))
	{
		std::string what = vm["openai-find"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::find(what, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-text-to-speech"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::text_to_speech::Model model = vm["openai-tts-model"].as<openai::text_to_speech::Model>();
		openai::text_to_speech::Voice voice = vm["openai-voice"].as<openai::text_to_speech::Voice>();
		chain |= openai::text_to_speech(api_key, model, voice);
	}

	if (vm.count("openai-embed"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::embed::model model = vm["openai-embed-model"].as<openai::embed::model>();
		chain |= openai::embed(api_key, model);
		chain |= [](message_ptr msg, const message_callbacks& emit_message) -> continuation {
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
		};
	}

	chain |= [](message_ptr msg, const message_callbacks& emit_message) -> continuation {
		if (msg->is<std::exception_ptr>())
			std::clog << "[WARNING] " <<
				errors::diagnostic_message(msg->get<std::exception_ptr>()) << std::endl;
		return emit_message(std::move(msg));
	};

	try
	{
		chain |= std::cout;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERROR] " <<
			errors::diagnostic_message(e) <<
			"processing file " + file_name << std::endl;
		return 2;
	}
  catch (...)
  {
		std::cerr << "[ERROR] Unknown error\nprocessing file " + file_name << std::endl;
		return 2;
  }

  return 0;
}
