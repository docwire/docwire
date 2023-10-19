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
#include <any>
#include <iostream>
#include <fstream>
#include "decompress_archives.h"
#include "exception.h"
#include "exporter.h"
#include "formatting_style.h"
#include "importer.h"
#include "log.h"
#include <magic_enum_iostream.hpp>
#include "standard_filter.h"
#include "transformer_func.h"
#include "version.h"
#include "chain_element.h"
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
	FormattingStyle formatting_style;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help message")
		("version", "display DocWire version")
		("verbose", "enable verbose logging")
		("input-file", po::value<std::string>()->required(), "path of file to process")
		("output_type", po::value<OutputType>()->default_value(OutputType::plain_text), enum_names_str<OutputType>().c_str())
		("language", po::value<Language>()->default_value(Language::eng), "")
		("min_creation_time", po::value<unsigned int>(), "")
		("max_creation_time", po::value<unsigned int>(), "")
		("max_nodes_number", po::value<unsigned int>(), "")
		("folder_name", po::value<std::string>(), "")
		("attachment_extension", po::value<std::string>(), "")
		("table-style", po::value<TableStyle>(&formatting_style.table_style)->default_value(TableStyle::table_look), enum_names_str<TableStyle>().c_str())
		("url-style", po::value<UrlStyle>(&formatting_style.url_style)->default_value(UrlStyle::extended), enum_names_str<UrlStyle>().c_str())
		("list-style-prefix", po::value<std::string>()->default_value(" * "), "")
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
		std::cout << std::endl << desc << std::endl;
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

	ParsingChain chain = Input(file_name) | DecompressArchives() | Importer(parameters, parser_manager);
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

  try
  {
	switch (vm["output_type"].as<OutputType>())
	{
		case OutputType::plain_text:
			chain | PlainTextExporter(std::cout);
			break;
		case OutputType::html:
			chain | HtmlExporter(std::cout);
			break;
		case OutputType::csv:
			chain | docwire::experimental::CsvExporter(std::cout);
			break;
		case OutputType::metadata:
			chain | MetaDataExporter(std::cout);
			break;
	}
  }
  catch (Exception& ex)
  {
      std::cout << "Error processing file " + file_name + ".\n" + ex.getBacktrace();
  }
  catch (...)
  {
    std::cout << "Error processing file " + file_name + ". Unknown error.\n";
  }

  return 0;
}
