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

#ifndef DOCWIRE_DOCWIRE_H
#define DOCWIRE_DOCWIRE_H

// IWYU pragma: begin_exports
#include "classify.h"
#include "content_type.h"
#include "content_type_by_file_extension.h"
#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"
#include "archives_parser.h"
#include "detect_sentiment.h"
#include "embed.h"
#include "exception_utils.h"
#include "find.h"
#include "fuzzy_match.h"
#include "input.h"
#include "output.h"
#include "mail_parser.h"
#include "model_chain_element.h"
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "plain_text_exporter.h"
#include "plain_text_writer.h"
#include "html_exporter.h"
#include "parsing_chain.h"
#include "summarize.h"
#include "text_to_speech.h"
#include "transcribe.h"
#include "transformer_func.h"
#include "translate_to.h"
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

#endif
