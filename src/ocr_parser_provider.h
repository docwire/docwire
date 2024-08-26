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

#ifndef DOCWIRE_OCR_PARSER_PROVIDER_H
#define DOCWIRE_OCR_PARSER_PROVIDER_H

#include "ocr_parser.h"
#include "parser_provider.h"

namespace docwire
{

using OcrParserProvider = parser_provider<OCRParser>;

} // namespace docwire

#endif //DOCWIRE_OCR_PARSER_PROVIDER_H
