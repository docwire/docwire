/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ODFOOXML_PARSER_H
#define DF0_ODFOOXML_PARSER_H

#include "audit.hpp"
#include "common_xml_document_parser.h"
#include "data_source.h"
#include "odf_ooxml_export.h"
#include "safety_policy.h"
#include <concepts>

namespace df0
{

/**
 * @brief A parser for ODF and OOXML document formats.
 * @tparam safety_level The safety policy to use.
 * @tparam auditor_type The injected auditor type.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
class DF0_ODF_OOXML_EXPORT odf_ooxml_parser
    : public common_xml_document_parser<safety_level, auditor_type>,
      public pipeline::transformer_element<odf_ooxml_parser<safety_level, auditor_type>>,
      public with_pimpl<odf_ooxml_parser<safety_level, auditor_type>>
{
    using base_type = common_xml_document_parser<safety_level, auditor_type>;
    using with_pimpl<odf_ooxml_parser<safety_level, auditor_type>>::impl;
    friend pimpl_impl<odf_ooxml_parser<safety_level, auditor_type>>;

    using base_type::registerODFOOXMLCommandHandler;
    using base_type::blanks;
    using base_type::set_blanks;
    using base_type::extractText;
    using base_type::parseODFMetadata;
    using base_type::parseXmlChildren;
    using base_type::getSharedStrings;
    using base_type::activeEmittingSignals;
    using base_type::auditor;
    using shared_string = base_type::shared_string;
    using scoped_context_stack_push = base_type::scoped_context_stack_push;

    class CommandHandlersSet;
    int lastOOXMLRowNum();
    void setLastOOXMLRowNum(int r);
    int lastOOXMLColNum();
    void setLastOOXMLColNum(int c);
    void parse(const data_source& data, xml_parse_mode mode, const message_callbacks& emit_message);
    attributes::metadata metaData(zip_reader& zipfile) const;

	public:

    /**
     * @brief Parses the given data source.
     * @param data The data source to parse.
     * @param emit_message Callback to emit messages (e.g., document elements).
     */
    void parse(const data_source& data, const message_callbacks& emit_message);

    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    odf_ooxml_parser()
        : odf_ooxml_parser(audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit odf_ooxml_parser(audit::auditor_ref<auditor_type> auditor);
    /**
     * @brief Processes a message in the parsing chain.
     * @return The continuation status.
     */
    continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

// Deduction guide for CTAD
odf_ooxml_parser() -> odf_ooxml_parser<default_safety_level, audit::null_auditor>;

} // namespace df0

#endif
