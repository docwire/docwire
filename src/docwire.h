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

#ifndef DOCWIRE_DOCWIRE_H
#define DOCWIRE_DOCWIRE_H

// IWYU pragma: begin_exports
#include "ai_runner.h"
#include "ai_summarize.h"
#include "ai_translate.h"
#include "ai_embed.h"
#include "ai_task.h"
#include "model_chain_element.h"
#ifdef DOCWIRE_CT2
#include "ct2_runner.h"
#endif
#ifdef DOCWIRE_LLAMA
#include "llama_runner.h"
#include "model_inference_config.h"
#endif
#ifdef DOCWIRE_LOCAL_AI
#include "local_ai_summarize.h"
#include "local_ai_translate.h"
#include "local_ai_embed.h"
#include "local_ai_task.h"
#endif
#include "ai_elements.h"
#include "classify.h"
#include "concepts.h"
#include "content_type.h"
#include "content_type_by_file_extension.h"
#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"
#include "convert.h"
#include "cosine_similarity.h"
#include "archives_parser.h"
#include "detect_sentiment.h"
#include "embed.h"
#include "ensure.h"
#include "environment.h"
#include "find.h"
#include "fuzzy_match.h"
#include "input.h"
#include "log.h"
#include "output.h"
#include "mail_elements.h"
#include "mail_parser.h"
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "plain_text_exporter.h"
#include "plain_text_writer.h"
#include "html_exporter.h"
#include "parsing_chain.h"
#include "serialization.h"
#include "summarize.h"
#include "text_to_speech.h"
#include "transcribe.h"
#include "transformer_func.h"
#include "translate_to.h"
#include "type_name.h"
#include "xml.h"
// IWYU pragma: end_exports

/**
 * @example integration_example.cmake
 * This example presents how to integrate DocWire SDK with your project using CMake
*/

/**
 * @example handling_errors_and_warnings.cpp
 * This example presents how to catch exceptions to handle critical errors and
 * how to use transformer to process non-critical errors (warnings) flowing through the pipeline
*/

/**
 * @example file_type_determination.cpp
 * This example shows how to perform file type detection (with or without document processing)
 * by file extension or file signature or rely on provided mime-type
*/

/**
 * @example path_to_text_stream.cpp
 * This example shows how to parse a document from a file path and export its plain text content to a stream.
*/

/**
 * @example stream_to_html.cpp
 * This example shows how to parse a document from a stream and export its content to HTML.
*/

/**
 * @example parse_archives.cpp
 * This example demonstrates how to recursively parse files contained within an archive (e.g., ZIP, TAR).
*/

/**
 * @example local_ai_classify.cpp
 * This example shows how to classify a document into one of several categories using a built-in local AI model.
*/

/**
 * @example openai_classify.cpp
 * This example shows how to classify a document using the OpenAI API.
*/

/**
 * @example local_ai_translate.cpp
 * This example demonstrates how to translate a document to another language using a built-in local AI model.
*/

/**
 * @example openai_translate.cpp
 * This example shows how to translate a document using the OpenAI API.
*/

/**
 * @example local_ai_sentiment.cpp
 * This example shows how to detect the sentiment of a document using a built-in local AI model.
*/

/**
 * @example openai_sentiment.cpp
 * This example shows how to detect the sentiment of a document using the OpenAI API.
*/

/**
 * @example local_ai_summary.cpp
 * This example demonstrates how to create a summary of a document using a built-in local AI model.
*/

/**
 * @example openai_voice_summary.cpp
 * This example shows how to create a voice summary by chaining an OpenAI summarizer and a text-to-speech model.
*/

/**
 * @example openai_transcribe_summary.cpp
 * This example shows how to transcribe an audio file and then summarize the resulting text using the OpenAI API.
*/

/**
 * @example local_ai_find.cpp
 * This example demonstrates how to find a specific sentence in a document using a built-in local AI model.
*/

/**
 * @example openai_find_image.cpp
 * This example shows how to find objects within an image using the OpenAI API.
*/

/**
 * @example openai_embedding.cpp
 * This example shows how to generate a text embedding for a document using the OpenAI API.
*/

/**
 * @example local_embedding_similarity.cpp
 * This example demonstrates how to create embeddings for a document and queries using a local AI model and then calculate their similarity.
*/

/**
 * @example reuse_chain.cpp
 * This example shows how to create a single processing chain and reuse it to parse multiple input files.
*/

/**
 * @example filter_emails_by_subject.cpp
 * This example shows how to use a transformer to filter emails from a PST file based on their subject line.
*/

/**
 * @example join_transformers.cpp
 * This example demonstrates how to join multiple transformers to create a more complex filter.
*/

/**
 * @example xml_parsing_example.cpp
 * This example demonstrates how to parse XML documents using the DocWire SDK modern C++ API.
*/

/**
 * @namespace docwire
 * @brief The main namespace for the DocWire SDK.
 */

#endif
