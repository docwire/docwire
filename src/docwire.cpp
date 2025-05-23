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

#include <boost/program_options.hpp>
#include <memory>
#include <fstream>
#include "analyze_data.h"
#include "classify.h"
#include "content_type.h"
#include "csv_exporter.h"
#include "decompress_archives.h"
#include "detect_sentiment.h"
#include "exception_utils.h"
#include "extract_entities.h"
#include "extract_keywords.h"
#include "find.h"
#include "html_exporter.h"
#include "language.h"
#include "log.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include "mail_parser.h"
#include "meta_data_exporter.h"
#include "model_chain_element.h"
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
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
	"/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */\n"
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

int main(int argc, char* argv[])
{
	bool local_processing;
	bool use_stream;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help message")
		("version", "display DocWire version")
		("verbose", "enable verbose logging")
		("input-file", po::value<std::string>()->required(), "path to file to process")
		("output_type", po::value<OutputType>()->default_value(OutputType::plain_text), enum_names_str<OutputType>().c_str())
		("http-post", po::value<std::string>(), "url to process data via http post")
		("local-ai-prompt", po::value<std::string>(), "prompt to process text via local AI model")
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
		("openai-transcribe", "convert speech to text (transcribe) via OpenAI")
		("openai-key", po::value<std::string>()->default_value(""), "OpenAI API key")
		("openai-model", po::value<openai::Model>()->default_value(openai::Model::gpt35_turbo), enum_names_str<openai::Model>().c_str())
		("openai-tts-model", po::value<openai::TextToSpeech::Model>()->default_value(openai::TextToSpeech::Model::tts1), enum_names_str<openai::TextToSpeech::Model>().c_str())
		("openai-voice", po::value<openai::TextToSpeech::Voice>()->default_value(openai::TextToSpeech::Voice::alloy), enum_names_str<openai::TextToSpeech::Voice>().c_str())
		("openai-temperature", po::value<float>(), "force specified temperature for OpenAI prompts")
		("openai-image-detail", po::value<openai::ImageDetail>()->default_value(openai::ImageDetail::automatic), enum_names_str<openai::ImageDetail>().c_str())
		("language", po::value<std::vector<Language>>()->default_value({Language::eng}, "eng"), "Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.")
		("local-processing", po::value<bool>(&local_processing)->default_value(true), "process documents locally including OCR")
		("use-stream", po::value<bool>(&use_stream)->default_value(false), "pass file stream to SDK instead of filename")
		("min_creation_time", po::value<unsigned int>(), "filter emails by min creation time")
		("max_creation_time", po::value<unsigned int>(), "filter emails by max creation time")
		("max_nodes_number", po::value<unsigned int>(), "filter by max number of nodes")
		("folder_name", po::value<std::string>(), "filter emails by folder name")
		("attachment_extension", po::value<std::string>(), "filter by attachment type")
		("log_file", po::value<std::string>(), "set path to log file")
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
		set_log_verbosity(debug);
	}

	std::unique_ptr<std::ostream> log_stream;
	if (vm.count("log_file"))
	{
		log_stream = std::make_unique<std::ofstream>(vm["log_file"].as<std::string>());
		set_log_stream(log_stream.get());
	}

	std::string file_name = vm["input-file"].as<std::string>();

	docwire_log_vars(use_stream, file_name);
	auto chain = use_stream ?
		(std::ifstream{file_name, std::ios_base::binary} | DecompressArchives()) :
		(std::filesystem::path{file_name} | DecompressArchives());

	if (vm.count("openai-transcribe"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		chain |= openai::Transcribe(api_key) | PlainTextExporter();
	}
	else if (local_processing)
	{
		chain |=
			content_type::detector{} |
			office_formats_parser{} | mail_parser{} | OCRParser{vm["language"].as<std::vector<Language>>()};
		if (vm.count("max_nodes_number"))
		{
			chain |= StandardFilter::filterByMaxNodeNumber(vm["max_nodes_number"].as<unsigned int>());
		}
		if (vm.count("min_creation_time"))
		{
			chain |= StandardFilter::filterByMailMinCreationTime(vm["min_creation_time"].as<unsigned int>());
		}
		if (vm.count("max_creation_time"))
		{
			chain |= StandardFilter::filterByMailMaxCreationTime(vm["max_creation_time"].as<unsigned int>());
		}
		if (vm.count("folder_name"))
		{
			chain |= StandardFilter::filterByFolderName({vm["folder_name"].as<std::string>()});
		}
		if (vm.count("attachment_extension"))
		{
			chain |= StandardFilter::filterByAttachmentType({file_extension{vm["attachment_extension"].as<std::string>()}});
		}

		switch (vm["output_type"].as<OutputType>())
		{
			case OutputType::plain_text:
				chain |= PlainTextExporter();
				break;
			case OutputType::html:
				chain |= HtmlExporter();
				break;
			case OutputType::csv:
				chain |= CsvExporter();
				break;
			case OutputType::metadata:
				chain |= MetaDataExporter();
				break;
		}
	}

	if (vm.count("http-post"))
	{
		chain |= http::Post(vm["http-post"].as<std::string>());
	}

	if (vm.count("openai-chat"))
	{
		std::string prompt = vm["openai-chat"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::Chat(prompt, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-extract-entities"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::ExtractEntities(api_key, model,
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
			openai::ExtractKeywords(max_keywords, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-summarize"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::Summarize(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-detect-sentiment"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::DetectSentiment(api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-analyze-data"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::AnalyzeData(api_key, model,
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
			openai::Classify(categories_set, api_key, model,
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
			openai::TranslateTo(language, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("local-ai-prompt"))
	{
		try
		{
			std::string prompt = vm["local-ai-prompt"].as<std::string>();

			auto model_runner = vm.count("local-ai-model") ?
				std::make_shared<local_ai::model_runner>(vm["local-ai-model"].as<std::string>()) :
				std::make_shared<local_ai::model_runner>();
			
			chain |=
				local_ai::model_chain_element(prompt, model_runner);
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << errors::diagnostic_message(e) << std::endl;
			return 1;
		}
	}

	if (vm.count("openai-find"))
	{
		std::string what = vm["openai-find"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ImageDetail image_detail = vm["openai-image-detail"].as<openai::ImageDetail>();
		chain |=
			openai::Find(what, api_key, model,
				vm.count("openai-temperature") ? vm["openai-temperature"].as<float>() : 0,
				image_detail);
	}

	if (vm.count("openai-text-to-speech"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::TextToSpeech::Model model = vm["openai-tts-model"].as<openai::TextToSpeech::Model>();
		openai::TextToSpeech::Voice voice = vm["openai-voice"].as<openai::TextToSpeech::Voice>();
		chain |= openai::TextToSpeech(api_key, model, voice);
	}

	chain |= [](Tag&& tag, const emission_callbacks& emit_tag) -> continuation {
		if (std::holds_alternative<std::exception_ptr>(tag))
			std::clog << "[WARNING] " <<
				errors::diagnostic_message(std::get<std::exception_ptr>(tag)) << std::endl;
		return emit_tag(std::move(tag));
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
