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


#ifndef DOCWIRE_OFFICE_FORMATS_PARSER_H
#define DOCWIRE_OFFICE_FORMATS_PARSER_H

#include "doc_parser.h"
#include "html_parser.h"
#include "iwork_parser.h"
#include "odfxml_parser.h"
#include "pdf_parser.h"
#include "xls_parser.h"
#include "xlsb_parser.h"
#include "odf_ooxml_parser.h"
#include "pipeline/composition.hpp"
#include "ppt_parser.h"
#include "rtf_parser.h"
#include "txt_parser.h"
#include "xml_parser.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <utility>

namespace docwire
{

/**
 * @brief A composite parser handling various office document formats.
 * @tparam safety_level The safety policy to use.
 * @tparam auditor_type The injected auditor type.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
using office_formats_parser = pipeline::composition<
    html_parser<auditor_type>,
    doc_parser<auditor_type>,
    pdf_parser<auditor_type>,
    xls_parser<auditor_type>,
    xlsb_parser<auditor_type>,
    iwork_parser<auditor_type>,
    ppt_parser<auditor_type>,
    rtf_parser<auditor_type>,
    odf_ooxml_parser<safety_level, auditor_type>,
    odfxml_parser<safety_level, auditor_type>,
    xml_parser<safety_level, auditor_type>,
    txt_parser<auditor_type>
>;

} // namespace docwire

#endif //DOCWIRE_OFFICE_FORMATS_PARSER_H
