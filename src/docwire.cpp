/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

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
#include "log.h"
#include <magic_enum_iostream.hpp>
#include "meta_data_exporter.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
#include "standard_filter.h"
#include "summarize.h"
#include "transformer_func.h"
#include "translate_to.h"
#include "version.h"
#include "parsing_chain.h"
#include "input.h"

using namespace docwire;

static void readme()
{
  // warning TODO: Generate readme string literal automatically.
  std::cout <<
       "***************************************************************************************************************************************************\n"
       "*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            *\n"
       "*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  *\n"
       "*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          *\n"
       "*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   *\n"
       "*                                                                                                                                                 *\n"
       "*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           *\n"
       "*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           *\n"
       "*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         *\n"
       "*                                                                                                                                                 *\n"
       "*  Copyright (c) SILVERCODERS Ltd                                                                                                                 *\n"
       "*  http://silvercoders.com                                                                                                                        *\n"
       "*                                                                                                                                                 *\n"
       "*  Project homepage:                                                                                                                              *\n"
       "*  https://github.com/docwire/docwire                                                                                                             *\n"
       "*  https://www.docwire.io/                                                                                                                        *\n"
       "*                                                                                                                                                 *\n"
       "*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                *\n"
       "*  the distribution and/or modification of this application.                                                                                      *\n"
       "*                                                                                                                                                 *\n"
       "*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               *\n"
       "*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         *\n"
       "*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    *\n"
       "*  Simply stop using the product if you disagree with this viewpoint.                                                                             *\n"
       "*                                                                                                                                                 *\n"
       "*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                *\n"
       "*  a current commercial license for this product may use this file.                                                                               *\n"
       "*                                                                                                                                                 *\n"
       "*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          *\n"
       "*  It is supplied in the hope that it will be useful.                                                                                             *\n"
       "***************************************************************************************************************************************************\n"
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
		("openai-key", po::value<std::string>()->default_value(""), "OpenAI API key")
		("openai-temperature", po::value<float>(), "force specified temperature for OpenAI prompts")
		("language", po::value<Language>()->default_value(Language::eng), "set document language for OCR")
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
	parameters += ParserParameters("language", vm["language"].as<Language>());

	docwire_log_vars(use_stream, file_name);
	std::ifstream in_stream;
	if (use_stream)
		in_stream.open(file_name, std::ios_base::binary);

	InputBase input = use_stream ? InputBase(&in_stream) : InputBase(file_name);

	ParsingChain chain = input | DecompressArchives() | Importer(parameters, parser_manager);

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
			chain | experimental::CsvExporter();
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
		openai::Chat chat = vm.count("openai-temperature") ?
			openai::Chat(prompt, api_key, vm["openai-temperature"].as<float>()) :
			openai::Chat(prompt, api_key);
		chain = chain | chat;
	}

	if (vm.count("openai-extract-entities"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::ExtractEntities extract_entities = vm.count("openai-temperature") ?
			openai::ExtractEntities(api_key, vm["openai-temperature"].as<float>()) :
			openai::ExtractEntities(api_key);
		chain = chain | extract_entities;
	}

	if (vm.count("openai-extract-keywords"))
	{
		unsigned int max_keywords = vm["openai-extract-keywords"].as<unsigned int>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::ExtractKeywords extract_keywords = vm.count("openai-temperature") ?
			openai::ExtractKeywords(max_keywords, api_key, vm["openai-temperature"].as<float>()) :
			openai::ExtractKeywords(max_keywords, api_key);
		chain = chain | extract_keywords;
	}

	if (vm.count("openai-summarize"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Summarize summarize = vm.count("openai-temperature") ?
			openai::Summarize(api_key, vm["openai-temperature"].as<float>()) :
			openai::Summarize(api_key);
		chain = chain | summarize;
	}

	if (vm.count("openai-detect-sentiment"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::DetectSentiment detect_sentiment = vm.count("openai-temperature") ?
			openai::DetectSentiment(api_key, vm["openai-temperature"].as<float>()) :
			openai::DetectSentiment(api_key);
		chain = chain | detect_sentiment;
	}

	if (vm.count("openai-analyze-data"))
	{
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::AnalyzeData analyze_data = vm.count("openai-temperature") ?
			openai::AnalyzeData(api_key, vm["openai-temperature"].as<float>()) :
			openai::AnalyzeData(api_key);
		chain = chain | analyze_data;
	}

	if (vm.count("openai-classify"))
	{
		const std::vector<std::string>& categories = vm["openai-classify"].as<std::vector<std::string>>();
		std::set<std::string> categories_set(categories.begin(), categories.end());
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::Classify classify = vm.count("openai-temperature") ?
			openai::Classify(categories_set, api_key, vm["openai-temperature"].as<float>()) :
			openai::Classify(categories_set, api_key);
		chain = chain | classify;
	}

	if (vm.count("openai-translate-to"))
	{
		std::string language = vm["openai-translate-to"].as<std::string>();
		std::string api_key = vm["openai-key"].as<std::string>();
		openai::TranslateTo translate_to = vm.count("openai-temperature") ?
			openai::TranslateTo(language, api_key, vm["openai-temperature"].as<float>()) :
			openai::TranslateTo(language, api_key);
		chain = chain | translate_to;
	}

	try
	{
		chain | Output(std::cout);
	}
catch (Exception& ex)
  {
      std::cout << "Error processing file " + file_name + ".\n" + ex.getBacktrace();
  }
	catch (const std::exception& e)
	{
		std::cout << "Error processing file " + file_name + ".\n" + e.what() << std::endl;
	}
  catch (...)
  {
    std::cout << "Error processing file " + file_name + ". Unknown error.\n";
  }

  return 0;
}
