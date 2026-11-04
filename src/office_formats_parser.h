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
#include <concepts>
#include <utility>

namespace df0
{

// Forward declaration so the detail alias can reference the class.
template <safety_policy safety_level, typename auditor_type>
class office_formats_parser;

namespace detail
{

template <safety_policy safety_level, typename auditor_type>
using office_formats_parser_base = pipeline::composition_wrapper<
    office_formats_parser<safety_level, auditor_type>,
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
    txt_parser<auditor_type>>;

} // namespace detail

/**
 * @brief Composite parser for common office document formats.
 *
 * This is a named composition of the individual format parsers. It can be
 * constructed with either:
 *
 * - `office_formats_parser{}` for the default no-op auditor, or
 * - `office_formats_parser<auditor_type>{auditor}` for an explicit auditor.
 *
 * @tparam safety_level XML safety policy for ODF/OOXML parsers.
 * @tparam auditor_type Injected auditor type.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
class office_formats_parser
    : public detail::office_formats_parser_base<safety_level, auditor_type>
{
public:
    using base_type = detail::office_formats_parser_base<safety_level, auditor_type>;

    /**
     * @brief Constructs the composite parser with the default stateless audit policy.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    office_formats_parser()
        : office_formats_parser(
              audit::auditor_ref<stateless_auditor_type>{})
    {}

    /**
     * @brief Constructs the composite parser with an explicit injected auditor.
     */
    explicit office_formats_parser(
        audit::auditor_ref<auditor_type> auditor)
        : base_type(auditor)
    {}
};

// Default deduction guide enabling `office_formats_parser{}`.
office_formats_parser() ->
    office_formats_parser<default_safety_level, audit::null_auditor>;

} // namespace df0

#endif //DOCWIRE_OFFICE_FORMATS_PARSER_H
