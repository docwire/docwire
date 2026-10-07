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

#ifndef DOCWIRE_MAIL_PARSER_H
#define DOCWIRE_MAIL_PARSER_H

#include "eml_parser.h"
#include "pipeline/chain_expression.hpp"
#include "pst_parser.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire
{

template<typename auditor_type = audit::null_auditor>
class mail_parser
    : public pipeline::transformer_element<mail_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    explicit mail_parser(audit::auditor_ref<auditor_type> auditor = {})
        : audit::with_auditor<auditor_type>(auditor),
          m_chain{eml_parser<auditor_type>{auditor}, pst_parser<auditor_type>{auditor}}
    {}

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        return m_chain(msg, emit_message);
    }

private:
    using audit::with_auditor<auditor_type>::auditor;
    pipeline::chain_expression<eml_parser<auditor_type>, pst_parser<auditor_type>> m_chain;
};

} // namespace docwire

#endif //DOCWIRE_MAIL_PARSER_H
