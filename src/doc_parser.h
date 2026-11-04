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

#ifndef DF0_DOC_PARSER_H
#define DF0_DOC_PARSER_H

#include "ole_office_formats_export.h"
#include "pipeline/element_base.hpp"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0
{

template<typename auditor_type = audit::null_auditor>
class DF0_OLE_OFFICE_FORMATS_EXPORT doc_parser
    : public pipeline::transformer_element<doc_parser<auditor_type>>,
      public with_pimpl<doc_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    doc_parser()
        : doc_parser(audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit doc_parser(audit::auditor_ref<auditor_type> auditor);

    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
    using with_pimpl<doc_parser<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;

    friend pimpl_impl<doc_parser<auditor_type>>;
};

// Deduction guide for CTAD
doc_parser() -> doc_parser<audit::null_auditor>;

} // namespace df0

#endif
