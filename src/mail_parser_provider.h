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

#ifndef DOCWIRE_MAIL_PARSER_PROVIDER_H
#define DOCWIRE_MAIL_PARSER_PROVIDER_H

#include "eml_parser.h"
#include "parser_provider.h"
#include "pst_parser.h"

namespace docwire
{

using MailParserProvider = parser_provider<EMLParser, PSTParser>;

} // namespace docwire

#endif //DOCWIRE_MAIL_PARSER_PROVIDER_H
