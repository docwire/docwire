/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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
#include "csv_exporter.h"
#include "decompress_archives.h"
#include "detect_sentiment.h"
#include "exception.h"
#include "extract_entities.h"
#include "extract_keywords.h"
#include "formatting_style.h"
#include "html_exporter.h"
#include "importer.h"
#include "language.h"
#include "log.h"
#include <magic_enum_iostream.hpp>
#include "meta_data_exporter.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
#include "standard_filter.h"
#include "summarize.h"
#include "text_to_speech.h"
#include "transcribe.h"
#include "transformer_func.h"
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
	"/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */\n"
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
	bool use_stream;
	FormattingStyle formatting_style;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help message")
		("version", "display DocWire version")
		("verbose", "enable verbose logging")
		("input-file", po::value<std::string>()->required(), "path to file to process")
		("output_type", po::value<OutputType>()->default_value(OutputType::plain_text), enum_names_str<OutputType>().c_str())
		("http-post", po::value<std::string>(), "url to process exported data via http post")
		("openai-chat", po::value<std::string>(), "prompt to process exported data via OpenAI")
		("openai-extract-entities", "extract entities from exported data via OpenAI")
		("openai-extract-keywords", po::value<unsigned int>(), "extract N keywords/key phrases from exported data via OpenAI")
		("openai-summarize", "summarize exported data via OpenAI")
		("openai-detect-sentiment", "detect sentiment of exported data via OpenAI")
		("openai-analyze-data", "analyze exported data for inportant insights and generate conclusions via OpenAI")
		("openai-classify", po::value<std::vector<std::string>>()->multitoken(), "classify exported data via OpenAI to one of specified categories")
		("openai-translate-to", po::value<std::string>(), "language to translate exported data to via OpenAI")
		("openai-text-to-speech", "convert text to speech via OpenAI")
		("openai-transcribe", "convert speech to text (transcribe) via OpenAI")
		("openai-key", po::value<std::string>()->default_value(""), "OpenAI API key")
		("openai-model", po::value<openai::Model>()->default_value(openai::Model::gpt35_turbo), enum_names_str<openai::Model>().c_str())
		("openai-tts-model", po::value<openai::TextToSpeech::Model>()->default_value(openai::TextToSpeech::Model::tts1), enum_names_str<openai::TextToSpeech::Model>().c_str())
		("openai-voice", po::value<openai::TextToSpeech::Voice>()->default_value(openai::TextToSpeech::Voice::alloy), enum_names_str<openai::TextToSpeech::Voice>().c_str())
		("openai-temperature", po::value<float>(), "force specified temperature for OpenAI prompts")
		("language", po::value<std::vector<Language>>()->default_value({Language::eng}, "eng"), "Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.")
		("use-stream", po::value<bool>(&use_stream)->default_value(false), "pass file stream to SDK instead of filename")
		("min_creation_time", po::value<unsigned int>(), "filter emails by min creation time")
		("max_creation_time", po::value<unsigned int>(), "filter emails by max creation time")
		("max_nodes_number", po::value<unsigned int>(), "filter by max number of nodes")
		("folder_name", po::value<std::string>(), "filter emails by folder name")
		("attachment_extension", po::value<std::string>(), "filter by attachment type")
		("table-style", po::value<TableStyle>(&formatting_style.table_style)->default_value(TableStyle::table_look), (enum_names_str<TableStyle>() + " (deprecated)").c_str())
		("url-style", po::value<UrlStyle>(&formatting_style.url_style)->default_value(UrlStyle::extended), (enum_names_str<UrlStyle>() + " (deprecated)").c_str())
		("list-style-prefix", po::value<std::string>()->default_value(" * "), "set output list prefix (deprecated)")
		("log_file", po::value<std::string>(), "set path to log file")
		("plugins_path", po::value<std::string>()->default_value(""), "set non-standard path to docwire plugins")
	;

	po::positional_options_description pos_desc;
	pos_desc.add("input-file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);

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

	po::notify(vm);

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

	std::shared_ptr<ParserManager> parser_manager { new ParserManager(vm["plugins_path"].as<std::string>()) };

	formatting_style.list_style.setPrefix(vm["list-style-prefix"].as<std::string>());

	ParserParameters parameters;
	parameters += ParserParameters("formatting_style", formatting_style);
	if (vm.count("language"))
	{
		const std::vector<Language>& languages = vm["language"].as<std::vector<Language>>();
		parameters += ParserParameters("languages", languages);
	}

	docwire_log_vars(use_stream, file_name);
	std::ifstream in_stream;
	if (use_stream)
		in_stream.open(file_name, std::ios_base::binary);

	InputBase input = use_stream ? InputBase(&in_stream) : InputBase(file_name);

	ParsingChain chain = input | DecompressArchives();
	if (vm.count("openai-transcribe"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		chain = chain | openai::Transcribe(api_key);
	}
	else
	{
		chain = chain | Importer(parameters, parser_manager);
	}

	if (vm.count("max_nodes_number"))
	{
		chain = chain | TransformerFunc(StandardFilter::filterByMaxNodeNumber(vm["max_nodes_number"].as<unsigned int>()));
	}
	if (vm.count("min_creation_time"))
	{
		chain = chain | TransformerFunc(StandardFilter::filterByMailMinCreationTime(vm["min_creation_time"].as<unsigned int>()));
	}
	if (vm.count("max_creation_time"))
	{
		chain = chain | TransformerFunc(StandardFilter::filterByMailMaxCreationTime(vm["max_creation_time"].as<unsigned int>()));
	}
	if (vm.count("folder_name"))
	{
		chain = chain | TransformerFunc(StandardFilter::filterByFolderName({vm["folder_name"].as<std::string>()}));
	}
	if (vm.count("attachment_extension"))
	{
		chain = chain | TransformerFunc(StandardFilter::filterByAttachmentType({vm["attachment_extension"].as<std::string>()}));
	}

	switch (vm["output_type"].as<OutputType>())
	{
		case OutputType::plain_text:
			chain | PlainTextExporter();
			break;
		case OutputType::html:
			chain | HtmlExporter();
			break;
		case OutputType::csv:
			chain | CsvExporter();
			break;
		case OutputType::metadata:
			chain | MetaDataExporter();
			break;
	}

	if (vm.count("http-post"))
	{
		chain = chain | http::Post(vm["http-post"].as<std::string>());
	}

	if (vm.count("openai-chat"))
	{
		std::string prompt = vm["openai-chat"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::Chat chat = vm.count("openai-temperature") ?
			openai::Chat(prompt, api_key, model, vm["openai-temperature"].as<float>()) :
			openai::Chat(prompt, api_key, model);
		chain = chain | chat;
	}

	if (vm.count("openai-extract-entities"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ExtractEntities extract_entities = vm.count("openai-temperature") ?
			openai::ExtractEntities(api_key, model, vm["openai-temperature"].as<float>()) :
			openai::ExtractEntities(api_key, model);
		chain = chain | extract_entities;
	}

	if (vm.count("openai-extract-keywords"))
	{
		unsigned int max_keywords = vm["openai-extract-keywords"].as<unsigned int>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::ExtractKeywords extract_keywords = vm.count("openai-temperature") ?
			openai::ExtractKeywords(max_keywords, api_key, model, vm["openai-temperature"].as<float>()) :
			openai::ExtractKeywords(max_keywords, api_key, model);
		chain = chain | extract_keywords;
	}

	if (vm.count("openai-summarize"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::Summarize summarize = vm.count("openai-temperature") ?
			openai::Summarize(api_key, model, vm["openai-temperature"].as<float>()) :
			openai::Summarize(api_key, model);
		chain = chain | summarize;
	}

	if (vm.count("openai-detect-sentiment"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::DetectSentiment detect_sentiment = vm.count("openai-temperature") ?
			openai::DetectSentiment(api_key, model, vm["openai-temperature"].as<float>()) :
			openai::DetectSentiment(api_key, model);
		chain = chain | detect_sentiment;
	}

	if (vm.count("openai-analyze-data"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::AnalyzeData analyze_data = vm.count("openai-temperature") ?
			openai::AnalyzeData(api_key, model, vm["openai-temperature"].as<float>()) :
			openai::AnalyzeData(api_key, model);
		chain = chain | analyze_data;
	}

	if (vm.count("openai-classify"))
	{
		const std::vector<std::string>& categories = vm["openai-classify"].as<std::vector<std::string>>();
		std::set<std::string> categories_set(categories.begin(), categories.end());
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::Classify classify = vm.count("openai-temperature") ?
			openai::Classify(categories_set, api_key, model, vm["openai-temperature"].as<float>()) :
			openai::Classify(categories_set, api_key, model);
		chain = chain | classify;
	}

	if (vm.count("openai-translate-to"))
	{
		std::string language = vm["openai-translate-to"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Model model = vm["openai-model"].as<openai::Model>();
		openai::TranslateTo translate_to = vm.count("openai-temperature") ?
			openai::TranslateTo(language, api_key, model, vm["openai-temperature"].as<float>()) :
			openai::TranslateTo(language, api_key, model);
		chain = chain | translate_to;
	}

	if (vm.count("openai-text-to-speech"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::TextToSpeech::Model model = vm["openai-tts-model"].as<openai::TextToSpeech::Model>();
		openai::TextToSpeech::Voice voice = vm["openai-voice"].as<openai::TextToSpeech::Voice>();
		chain = chain | openai::TextToSpeech(api_key, model, voice);
	}

	try
	{
		chain | Output(std::cout);
	}
catch (Exception& ex)
  {
		std::cerr << "Error processing file " + file_name + ".\n" + ex.getBacktrace();
  }
	catch (const std::exception& e)
	{
		std::cerr << "Error processing file " + file_name + ".\n" + e.what() << std::endl;
	}
  catch (...)
  {
		std::cerr << "Error processing file " + file_name + ". Unknown error.\n";
  }

  return 0;
}
