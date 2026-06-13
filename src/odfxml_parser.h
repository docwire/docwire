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

#ifndef DOCWIRE_ODFXML_PARSER_H
#define DOCWIRE_ODFXML_PARSER_H

#include "common_xml_document_parser.h"
#include "odf_ooxml_export.h"
#include "pimpl.h"
#include "safety_policy.h"

namespace docwire
{

/**
 * @brief A parser for flat ODF XML documents.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level>
class DOCWIRE_ODF_OOXML_EXPORT odfxml_parser : public common_xml_document_parser<safety_level>, public with_pimpl<odfxml_parser<safety_level>>
{
	private:
		using base_type = common_xml_document_parser<safety_level>;
		using with_pimpl<odfxml_parser<safety_level>>::impl;
		friend pimpl_impl<odfxml_parser<safety_level>>;

		using base_type::registerODFOOXMLCommandHandler;
		using scoped_context_stack_push = base_type::scoped_context_stack_push;

	protected:
		auto create_base_context_guard(const message_callbacks& emit_message)
		{
			return scoped_context_stack_push{*this, emit_message};
		}

	public:
		/**
		 * @brief Default constructor.
		 */
		odfxml_parser();
		/**
		 * @brief Processes a message in the parsing chain.
		 * @return The continuation status.
		 */
		continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
