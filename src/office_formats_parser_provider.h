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


#ifndef DOCWIRE_OFFICE_FORMATS_PARSER_PROVIDER_H
#define DOCWIRE_OFFICE_FORMATS_PARSER_PROVIDER_H

#include "doc_parser.h"
#include "html_parser.h"
#include "iwork_parser.h"
#include "odfxml_parser.h"
#include "pdf_parser.h"
#include "xls_parser.h"
#include "xlsb_parser.h"
#include "odf_ooxml_parser.h"
#include "parser_provider.h"
#include "ppt_parser.h"
#include "rtf_parser.h"
#include "txt_parser.h"
#include "xml_parser.h"

namespace docwire
{

using OfficeFormatsParserProvider = parser_provider<HTMLParser, DOCParser, PDFParser, XLSParser, XLSBParser, IWorkParser, PPTParser, RTFParser, ODFOOXMLParser, ODFXMLParser, XMLParser, TXTParser>;

} // namespace docwire

#endif //DOCWIRE_OFFICE_FORMATS_PARSER_PROVIDER_H
