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
#include "standard_filter.h"
#include "transformer_func.h"
#include "version.h"
#include "chain_element.h"
#include "parsing_chain.h"
#include "input.h"

using namespace docwire;

FormattingStyle updateFormattingStyle(const std::string &arg, const FormattingStyle formatting_style)
{
  FormattingStyle updated_formatting_style{formatting_style};
  if(arg.find("table-style=one-row", 0) != std::string::npos)
  {
    updated_formatting_style.table_style = TABLE_STYLE_ONE_ROW;
  }
  else if(arg.find("table-style=one-col", 0) != std::string::npos)
  {
    updated_formatting_style.table_style = TABLE_STYLE_ONE_COL;
  }
  else if(arg.find("table-style=table-look", 0) != std::string::npos)
  {
    updated_formatting_style.table_style = TABLE_STYLE_TABLE_LOOK;
  }
  if(arg.find("url-style=text-only", 0) != std::string::npos)
  {
    updated_formatting_style.url_style = URL_STYLE_TEXT_ONLY;
  }
  if(arg.find("url-style=extended", 0) != std::string::npos)
  {
    updated_formatting_style.url_style = URL_STYLE_EXTENDED;
  }
  if(arg.find("url-style=underscored", 0) != std::string::npos)
  {
    updated_formatting_style.url_style = URL_STYLE_UNDERSCORED;
  }
  if(arg.find("list-style-prefix=", 0) != std::string::npos)
  {
    updated_formatting_style.list_style.setPrefix(arg.substr(arg.find("list-style-prefix=", 0) + 18));
  }
  return updated_formatting_style;
}

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

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    version();
    return 0;
  }

  std::string file_name = argv[1];

  FormattingStyle formatting_style;
  enum class OutputType { PLAIN_TEXT, HTML, CSV, METADATA };
  OutputType output_type {OutputType::PLAIN_TEXT};
  Language language{ Language::english };
  std::string plugins_path = "";

  std::optional<unsigned int> min_creation_time;
  std::optional<unsigned int> max_creation_time;
  std::optional<unsigned int> max_nodes_number;
  std::optional<std::string> folder_name;
  std::optional<std::string> log_file_name;
  std::optional<std::string> attachment_extension;

  ParserParameters parameters;
  for (unsigned int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    formatting_style = updateFormattingStyle(arg, formatting_style);
    if (arg.find("--max_nodes_number", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      max_nodes_number = std::stoi(argv[i + 1]);
      ++i;
      continue;
    }
    if (arg.find("--plugins_path", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      plugins_path = argv[++i];
      continue;
    }
    if (arg.find("--html_output", 0) != -1)
    {
      output_type = OutputType::HTML;
      continue;
    }
    if (arg.find("--csv_output", 0) != -1)
    {
      output_type = OutputType::CSV;
      continue;
    }
    if (arg.find("--meta", 0) != -1)
    {
      output_type = OutputType::METADATA;
      continue;
    }
    if (arg.find("--min_creation_time", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      min_creation_time = std::stoi(argv[i + 1]);
    }
    if (arg.find("--max_creation_time", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      max_creation_time = std::stoi(argv[i + 1]);
    }
    if (arg.find("--folder_name", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      folder_name = argv[i + 1];
    }
    if (arg.find("--attachment_extension", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      attachment_extension = argv[i + 1];
    }
    if (arg.find("--language", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      language = nameToLanguage(argv[i + 1]);
    }
    if (arg.find("--verbose", 0) != -1)
    {
      set_log_verbosity(debug);
    }
    if (arg.find("--log-file", 0) != -1)
    {
      if (i >= argc - 1)
      {
        throw std::runtime_error("incorrect input parameters");
      }
      log_file_name = argv[i + 1];
    }
    formatting_style.list_style.setPrefix(" * ");
  }
  std::shared_ptr<ParserManager> parser_manager { new ParserManager(plugins_path) };
  parameters += ParserParameters("formatting_style", formatting_style);

  parameters += ParserParameters("language", language);

  std::unique_ptr<std::ostream> log_stream;

  if (log_file_name)
  {
    log_stream = std::make_unique<std::ofstream>(*log_file_name);
    set_log_stream(log_stream.get());
  }

  ParsingChain chain = Input(file_name) | DecompressArchives() | Importer(parameters, parser_manager);
  if (max_nodes_number)
  {
    chain = chain | TransformerFunc(StandardFilter::filterByMaxNodeNumber(*max_nodes_number));
  }
  if (min_creation_time)
  {
    chain = chain | TransformerFunc(StandardFilter::filterByMailMinCreationTime(*min_creation_time));
  }
  if (max_creation_time)
  {
    chain = chain | TransformerFunc(StandardFilter::filterByMailMaxCreationTime(*max_creation_time));
  }
  if (folder_name)
  {
    chain = chain | TransformerFunc(StandardFilter::filterByFolderName({*folder_name}));
  }
  if (attachment_extension)
  {
    chain = chain | TransformerFunc(StandardFilter::filterByAttachmentType({*attachment_extension}));
  }
  try
  {
      switch (output_type)
      {
        case OutputType::PLAIN_TEXT:
          chain | PlainTextExporter(std::cout);
          break;
        case OutputType::HTML:
          chain | HtmlExporter(std::cout);
          break;
        case OutputType::CSV:
          chain | docwire::experimental::CsvExporter(std::cout);
          break;
        case OutputType::METADATA:
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
