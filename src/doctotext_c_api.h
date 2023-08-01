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

/**
 * @file doctotext_c_api.h
 * @brief File contains c api for doctotext software
 *
 */

#ifndef DOCTOTEXT_C_API_H
#define DOCTOTEXT_C_API_H

#include <stdbool.h>
#include "defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DOCTOTEXT_CALL
  #if defined(_WIN32) && !defined(_WIN64)
    #define DOCTOTEXT_CALL __cdecl
  #else
    #define DOCTOTEXT_CALL
  #endif
#endif

typedef struct DocToTextParserManager DocToTextParserManager; ///< @see doctotext::ParserManager
typedef struct DocToTextItem DocToTextItem;
typedef struct DocToTextParser DocToTextParser; ///< @see doctotext::Parser
typedef struct DocToTextInfo DocToTextInfo; ///< @see doctotext::Info
typedef struct DocToTextParameters DocToTextParameters; ///< @see doctotext::ParserParameters
typedef struct DocToTextWriter DocToTextWriter; ///< @see doctotext::Writer

typedef struct DocToTextInput DocToTextInput; ///< @see doctotext::Input
typedef struct DocToTextImporter DocToTextImporter; ///< @see doctotext::Importer
typedef struct DocToTextExporter DocToTextExporter; ///< @see doctotext::Exporter
typedef struct DocToTextTransformer DocToTextTransformer; ///< @see doctotext::Transformer
typedef struct DocToTextParsingChain DocToTextParsingChain; ///< @see doctotext::ParsingChain
typedef struct DocToTextSimpleExtractor DocToTextSimpleExtractor; ///< @see doctotext::SimpleExtractor

/**
 * @brief Creates a new DocToTextSimpleExtractor object.
 * Example:
 * @snippet example_8.c example_c
 * @param file_name The name of the file to be parsed.
 * @return The new DocToTextSimpleExtractor object.
 */
DllExport DocToTextSimpleExtractor*
DOCTOTEXT_CALL doctotext_create_simple_extractor(const char* file_name);

/**
 * @brief Gets parsed plain text from a DocToTextSimpleExtractor object.
 * @param extractor The DocToTextSimpleExtractor object.
 * @return The parsed plain text.
 */
DllExport const char *
DOCTOTEXT_CALL doctotext_simple_extractor_get_plain_text(DocToTextSimpleExtractor* extractor);

/**
 * @brief Adds a callback function to be called during parsing.
 * Example of usage:
 * @snippet example_8.c example_c
 * @param extractor The DocToTextSimpleExtractor object.
 * @param callback The callback function.
 * @param data The data to be passed to the callback function.
 */
DllExport void
DOCTOTEXT_CALL doctotext_simple_extractor_add_callback_function(DocToTextSimpleExtractor* extractor, void (*callback)(DocToTextInfo*, void* data), void* data);

/**
 * @brief Creates a new DocToTextInput object. This object is used to wrap filename or stream.
 * @param file_name path to the file
 * @return
 */
DllExport DocToTextInput*
DOCTOTEXT_CALL doctotext_create_input_from_file_name(const char *file_name);

/**
 * @brief Creates a new DocToTextInput object. This object is used to wrap filename or stream.
 * @param input_stream stream with input data to parse
 * @return
 */
DllExport DocToTextInput*
DOCTOTEXT_CALL doctotext_create_input_from_stream(FILE *input_stream);

/**
 * @brief Creates a new DocToTextImporter object. This object is used to import a file and parse it using available parsers.
 * Properly parser is selected based on file extension.
 * @param manager parser manager
 * @return
 */
DllExport DocToTextImporter*
DOCTOTEXT_CALL doctotext_create_importer(DocToTextParserManager *manager);

/**
 * @brief Creates a new DocToTextExporter object. This object is used to export parsed data to output as a plain text.
 * @param manager
 * @return new DocToTextExporter object
 */
DllExport DocToTextExporter*
DOCTOTEXT_CALL doctotext_create_plain_text_exporter(FILE *output_stream);

/**
 * @brief Creates a new DocToTextExporter object. This object is used to export parsed data to output as a html.
 * @param manager
 * @return new DocToTextExporter object
 */
DllExport DocToTextExporter*
DOCTOTEXT_CALL doctotext_create_html_exporter(FILE *output_stream);

/**
 * @brief Creates connection between input and importer and returns DocToTextParsingChain which
 * contains all defined steps of the parsing chain.
 * @param input
 * @param importer
 * @return new ParsingChain object
 */
DllExport DocToTextParsingChain*
DOCTOTEXT_CALL doctotext_connect_input_to_importer(DocToTextInput* input, DocToTextImporter* importer);

/**
 * @brief Creates connection between importer and exporter and returns DocToTextParsingChain which
 * contains all defined steps of the parsing chain.
 * @param importer
 * @param exporter
 * @return new ParsingChain object
 */
DllExport DocToTextParsingChain*
DOCTOTEXT_CALL doctotext_connect_importer_to_exporter(DocToTextImporter* importer, DocToTextExporter *exporter);

/**
 * @brief Creates a new DocToTextTransformer object. This object is used to transform parsed data.
 * Example of usage:
 * @snippet example_3.c example_c
 * @param callback function to be called during transformation
 * @param data data to be passed to the callback function
 * @return new DocToTextTransformer object
 */
DllExport DocToTextTransformer*
DOCTOTEXT_CALL doctotext_create_transfomer(void (*callback)(DocToTextInfo*, void* data), void* data);

/**
 * @brief Creates connection between importer and transformer and returns DocToTextParsingChain which
 * contains all defined steps of the parsing chain.
 * @param importer
 * @param transformer
 * @return new ParsingChain object
 */
DllExport DocToTextParsingChain*
DOCTOTEXT_CALL doctotext_connect_importer_to_transformer(DocToTextImporter* importer, DocToTextTransformer *transformer);

/**
 * @brief Adds transformer to the parsing chain.
 * @param parsing_chain ParsingChain object
 * @param transformer DocToTextTransformer object
 * @return parsing_chain with added transformer
 */
DllExport DocToTextParsingChain*
DOCTOTEXT_CALL doctotext_connect_parsing_chain_to_transformer(DocToTextParsingChain *parsing_chain, DocToTextTransformer *transformer);

/**
 * @brief Adds exporter to the parsing chain.
 * @param parsing_chain ParsingChain object
 * @param exporter DocToTextExporter object
 * @return parsing_chain with added exporter
 */
DllExport DocToTextParsingChain*
DOCTOTEXT_CALL doctotext_connect_parsing_chain_to_exporter(DocToTextParsingChain *parsing_chain, DocToTextExporter *exporter);

/**
 * @brief Adds input stream to the parsing chain. This function starts parsing chain.
 * @param parsing_chain ParsingChain object
 * @param input_stream input stream
 */
DllExport void
DOCTOTEXT_CALL doctotext_parsing_chain_set_input(DocToTextParsingChain *parsing_chain, FILE* input_stream);

/**
 * @brief Frees input and all resources allocated by the input. DocToTextInput is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_input (which uses
 * operator delete).
 * @param importer
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_input(DocToTextInput *input);

/**
 * @brief Frees importer and all resources allocated by the importer. DocToTextImporter is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_importer (which uses
 * operator delete).
 * @param importer
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_importer(DocToTextImporter *importer);

/**
 * @brief Frees exporter and all resources allocated by the exporter. Remember not to use function free(). DocToTextExporter is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_exporter (which uses
 * operator delete).
 * @param exporter
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_exporter(DocToTextExporter *exporter);

/**
 * @brief Frees transformer and all resources allocated by the transformer. Remember not to use function free(). DocToTextTransformer is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_transformer (which uses
 * operator delete).
 * @param transformer
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_transformer(DocToTextTransformer *transformer);

/**
 * @brief Frees parsing_chain and all resources allocated by the parsing chain. Remember not to use function free(). DocToTextParsingChain is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_parsing_chain (which uses
 * operator delete).
 * @param parsing_chain
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_parsing_chain(DocToTextParsingChain *parsing_chain);

/**
 * @brief Creates new parser manager with all available parsers.
 * @param path_to_plugins Path to plugins directory.
 * @return Handle to new parser
 */
DllExport DocToTextParserManager*
DOCTOTEXT_CALL doctotext_init_parser_manager(const char* path_to_plugins);

/**
 *
 * @param parser_manager
 * @param formats_number number of supported formats
 * @return names of all supported formats
 */
DllExport char**
doctotext_parser_manager_get_available_formats(DocToTextParserManager* parser_manager, unsigned int *formats_number);

/**
 * @brief Returns proper parser for given format. The format is defined by file extension.
 * Example of usage:
 * @snippet example_5.c example_c
 * @param parser_manager
 * @param format
 * @return parser for given format
 */
DllExport DocToTextParser*
DOCTOTEXT_CALL doctotext_parser_manager_get_parser_by_extension(DocToTextParserManager* parser_manager, const char *format);

/**
 * @brief Adds new function to execute when new node will be parsed. Node is a part of hierarchical structure. For example
 * it could be a single file in a zip file or a single email in pst file. In case of plain structure node is an entire file.
 *
 * @code
 * bool print_in_terminal;
 *
 * void onNewNodeCallback(DocToTextInfo* info, void* data)
 * {
 *   const char* text = doctotext_info_get_plain_text(info);
 *   bool* print_in_terminal = (bool*)(data)
 *   if ((*print_in_terminal) == true)
 *   {
 *      printf(text);
 *   }
 * }
 *
 * doctotext_parser_add_callback_on_new_node(parser, &onNewNodeCallback, &print_in_terminal);
 *
 * @endcode
 *
 * @param parser
 * @param callback
 * @param data this pointer to data will be passed as an output parameter in callback function
 */
DllExport void
DOCTOTEXT_CALL doctotext_parser_add_callback_on_new_node(DocToTextParser* parser, void (*callback)(DocToTextInfo*, void* data), void* data);

/**
 * @brief Adds DocToTextParameters to parser. Every parser pass recursively DocToTextParameters to another parsers.
 * @param parser
 * @param parameter
 */
DllExport void
DOCTOTEXT_CALL doctotext_parser_add_parameters(DocToTextParser* parser, DocToTextParameters* parameters);

/**
 * @brief Start parsing loaded data. The data comes from file or from buffer.
 * @param parser
 */
DllExport void
DOCTOTEXT_CALL doctotext_parser_parse(DocToTextParser* parser);

/**
 * @brief Frees parser. Remember not to use function free(). DocToTextParser is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_parser (which uses
 * operator delete).
 * @param parser
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_parser(DocToTextParser* parser);

/**
 * @brief Returns parsed text from DocToTextInfo
 * @param info callback parameter. It contains information about parsed text.
 * @return parsed text
 */
DllExport const char*
DOCTOTEXT_CALL doctotext_info_get_plain_text(DocToTextInfo* info);

/**
 *
 * @param info
 * @return
 */
DllExport const char*
DOCTOTEXT_CALL doctotext_info_get_tag_name(DocToTextInfo* info);

/**
 * @brief Returns attribute value as a string from DocToTextInfo
 * @param info callback parameter. It contains information about parsed text.
 * @param attribute_name name of attribute
 * @return attribute value
 */
DllExport const char*
DOCTOTEXT_CALL doctotext_info_get_string_attribute(DocToTextInfo* info, const char* attribute_name);

/**
 * @brief Returns attribute value as a unsigned integer from DocToTextInfo
 * @param info callback parameter. It contains information about parsed text.
 * @param attribute_name name of attribute
 * @return attribute value
 */
DllExport unsigned int
DOCTOTEXT_CALL doctotext_info_get_uint_attribute(DocToTextInfo* info, const char* attribute_name);

/**
 * @brief Sets cancel flag in DocToTextInfo. If cancel is true then parsing chain will be stop.
 * Example of usage:
 * @code
 * bool stop_parser;
 *
 * void onNewNodeCallback(DocToTextInfo* info, void* data)
 * {
 *   const char* text = doctotext_info_get_plain_text(info);
 *   bool* stop_parser = (bool*)(data)
 *   doctotext_info_set_cancel_parser(info, (*stop_parser));
 * }
 *
 * doctotext_parser_add_callback_on_new_node(parser, &onNewNodeCallback, &stop_parser);
 *
 * @endcode
 * @param info input/output structure from callback
 * @param cancel
 */
DllExport void
DOCTOTEXT_CALL doctotext_info_set_cancel_parser(DocToTextInfo* info, bool cancel);

/**
 * @brief Sets skip flag in DocToTextInfo. If skip is true then current node will be skipped.
 * Example of usage:
 * @snippet example_6.c example_c
 * @param info input/output structure from callback
 * @param skip true if node should be skipped
 */
DllExport void
DOCTOTEXT_CALL doctotext_info_set_skip(DocToTextInfo* info, bool skip);

/**
 * @brief Creates new empty DocToTextParameters. In next step we can pass to DocToTextParameters required parameters like
 * for example min_creation_time or max_creation_time.
 * Example od usage:
 * @code
 * DocToTextParameters* parameters = doctotext_create_parameter(); // create empty DocToTextParameters
 * doctotext_add_uint_parameter(parameters, "min_creation_time", 1234123); // add min_creation_time parameter
 * doctotext_add_uint_parameter(parameters, "max_creation_time", 1834123); // add min_creation_time parameter
 * doctotext_parser_add_parameter(parser, parameters); // pass all parameters to parser
 * @endcode
 * @return new DocToTextParameters
 */
DllExport DocToTextParameters*
DOCTOTEXT_CALL doctotext_create_parameter();

/**
 * @brief Adds int parameter to parser parameters
 * @param parameters pointer to parser parameters
 * @param name name of parameter
 * @param value value of parameter
 */
DllExport void
DOCTOTEXT_CALL doctotext_add_int_parameter(DocToTextParameters* parameters, const char* name, int value);

/**
 * @brief Adds unsigned int parameter to parser parameters
 * @param parameters pointer to parser parameters
 * @param name name of parameter
 * @param value value of parameter
 */
DllExport void
DOCTOTEXT_CALL doctotext_add_uint_parameter(DocToTextParameters* parameters, const char* name, unsigned int value);

/**
 * @brief Adds float parameter to parser parameters
 * @param parameters pointer to parser parameters
 * @param name name of parameter
 * @param value value of parameter
 */
DllExport void
DOCTOTEXT_CALL doctotext_add_float_parameter(DocToTextParameters* parameters, const char* name, float value);


/**
 * @brief Adds const char* parameter to parser parameters
 * @param parameters pointer to parser parameters
 * @param name name of parameter
 * @param value value of parameter
 */
DllExport void
DOCTOTEXT_CALL doctotext_add_string_parameter(DocToTextParameters* parameters, const char* name, const char* value);

/**
 * @brief Creates HtmlWriter. HtmlWriter writes parsed date from callbacks as html.
 * Example of usage:
 * @code
 * void onNewNodeCallback(DocToTextInfo* info, void* data)
 * {
 *   HtmlWriter* writer = (HtmlWriter*)(data);
 *   const char* html_text = doctotext_html_writer_write(writer, info);
 *   printf("%s", html_text); // print parts of html from callback output
 * }
 *
 * HtmlWriter* writer = doctotext_create_html_writer();
 * doctotext_parser_add_callback_on_new_node(parser, &onNewNodeCallback, writer);
 * doctotext_parser_parse(parser); // parse document
 * doctotext_free_html_writer(writer); // free writer
 * @endcode
 *
 * @return new HtmlWriter
 */
DllExport DocToTextWriter*
DOCTOTEXT_CALL doctotext_create_html_writer();

/**
 * @brief Creates PlainTextWriter. PlainTextWriter writes parsed data from callbacks as plain text.
 **/
DllExport DocToTextWriter*
DOCTOTEXT_CALL doctotext_create_plain_text_writer();

/**
 * @brief Frees HtmlWriter. DocToTextWriter is
 * allocated using operator new (from C++) and is supposed to be deleted by doctotext_free_html_writer (which uses
 * operator delete).
 * @param writer HtmlWriter to release
 */
DllExport void
DOCTOTEXT_CALL doctotext_free_writer(DocToTextWriter* writer);

/**
 * @brief Converts text from callback to html format
 * @param writer HtmlWriter
 * @param info input/output structure from callback
 */
DllExport void
DOCTOTEXT_CALL doctotext_writer_write(DocToTextWriter* writer, DocToTextInfo* info, FILE* out_stream);

#ifdef __cplusplus
}
#endif

#endif //DOCTOTEXT_C_API_H
