/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
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
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "doctotext_c_api.h"

struct callbackData
{
  int counter;
  int max_nodes_number;
  bool html_output;
};

void onNewNodeCallback(DocToTextInfo* info, void* data)
{
  struct callbackData* p_callback_data = (struct callbackData*)(data);

  if (p_callback_data->counter++ == p_callback_data->max_nodes_number)
  {
    doctotext_info_set_cancel_parser(info, true);
  }
  if (p_callback_data->html_output)
  {
    DocToTextWriter *writer = doctotext_create_html_writer();
    doctotext_writer_write(writer, info, stdout);
    doctotext_free_writer(writer);
  }
  else
  {
    printf(doctotext_info_get_plain_text(info));
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    return 0;
  }

  struct callbackData callback_data;
  callback_data.counter = 0;
  callback_data.max_nodes_number = -1;
  const char* plugins_path = "";

  callback_data.html_output = false;

  DocToTextParameters* parameters = doctotext_create_parameter();
  for (unsigned int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--max_nodes_number") == 0)
    {
      callback_data.max_nodes_number = strtol(argv[i + 1], NULL, 10);
    }
    if (strcmp(argv[i], "--min_creation_time") == 0)
    {
      doctotext_add_uint_parameter(parameters, "min_creation_time", (unsigned int)(argv[i + 1]));
    }
    if (strcmp(argv[i], "--max_creation_time") == 0)
    {
      doctotext_add_uint_parameter(parameters, "max_creation_time", (unsigned int)(argv[i + 1]));
    }
    if (strcmp(argv[i], "--html_output") == 0)
    {
      callback_data.html_output = true;
    }
    if (strcmp(argv[i], "--plugins_path") == 0)
    {
      plugins_path = argv[i + 1];
    }
  }

  DocToTextParserManager* parser_manager = doctotext_init_parser_manager(plugins_path);
  DocToTextParser* parser = doctotext_parser_manager_get_parser_by_extension(parser_manager, argv[1]);
  if (parser)
  {
    doctotext_parser_add_callback_on_new_node(parser, &onNewNodeCallback, &callback_data);
    doctotext_parser_add_parameters(parser, parameters);
    DocToTextWriter *writer = doctotext_create_plain_text_writer();
    doctotext_parser_parse(parser);
    doctotext_free_parser(parser);
  }
  else
  {
    printf("Unsupported file format\n");
    printf("Supported formats: \n");
    unsigned int formats_number;
    char** formats = doctotext_parser_manager_get_available_formats(parser_manager, &formats_number);
    for (unsigned int i = 0; i < formats_number; ++i)
    {
      char* format_name = formats[i];
      printf(format_name);
      printf("\n");
    }
  }
  return 0;
}
