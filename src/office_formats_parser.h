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
#include "pipeline/chain_expression.hpp"
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
class office_formats_parser
    : public pipeline::transformer_element<office_formats_parser<safety_level, auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    explicit office_formats_parser(audit::auditor_ref<auditor_type> auditor = {})
        : audit::with_auditor<auditor_type>(auditor),
          m_chain{make_chain(audit::auditor_ref<auditor_type>(auditor))}
    {}

    /**
     * @brief Executes the composite chain of format parsers.
     */
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        return m_chain(msg, emit_message);
    }

private:
    using audit::with_auditor<auditor_type>::auditor;

    static auto make_chain(audit::auditor_ref<auditor_type> auditor)
    {
        return html_parser<auditor_type>{auditor} |
            doc_parser<auditor_type>{auditor} |
            pdf_parser<auditor_type>{auditor} |
            xls_parser<auditor_type>{auditor} |
            xlsb_parser<auditor_type>{auditor} |
            iwork_parser{} |
            ppt_parser<auditor_type>{auditor} |
            rtf_parser<auditor_type>{auditor} |
            odf_ooxml_parser<safety_level, auditor_type>{auditor} |
            odfxml_parser<safety_level, auditor_type>{auditor} |
            xml_parser<safety_level, auditor_type>{auditor} |
            txt_parser<auditor_type>{parse_paragraphs{true}, parse_lines{true}, auditor};
    }

    using chain_type = decltype(make_chain(std::declval<audit::auditor_ref<auditor_type>>()));
    chain_type m_chain;
};

} // namespace docwire

#endif //DOCWIRE_OFFICE_FORMATS_PARSER_H
