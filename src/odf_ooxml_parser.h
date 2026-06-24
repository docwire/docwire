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

#ifndef DOCWIRE_ODFOOXML_PARSER_H
#define DOCWIRE_ODFOOXML_PARSER_H

#include "common_xml_document_parser.h"
#include "data_source.h"
#include "odf_ooxml_export.h"
#include "safety_policy.h"

namespace docwire
{

/**
 * @brief A parser for ODF and OOXML document formats.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level>
class DOCWIRE_ODF_OOXML_EXPORT odf_ooxml_parser : public common_xml_document_parser<safety_level>, public with_pimpl<odf_ooxml_parser<safety_level>>
{
    using base_type = common_xml_document_parser<safety_level>;
    using with_pimpl<odf_ooxml_parser<safety_level>>::impl;
    friend pimpl_impl<odf_ooxml_parser<safety_level>>;

    using base_type::registerODFOOXMLCommandHandler;
    using base_type::blanks;
    using base_type::set_blanks;
    using base_type::extractText;
    using base_type::parseODFMetadata;
    using base_type::parseXmlChildren;
    using base_type::getSharedStrings;
    using base_type::activeEmittingSignals;
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
     * @brief Default constructor.
     */
    odf_ooxml_parser();
    /**
     * @brief Processes a message in the parsing chain.
     * @return The continuation status.
     */
    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
    bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
