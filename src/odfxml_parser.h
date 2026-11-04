/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ODFXML_PARSER_H
#define DF0_ODFXML_PARSER_H

#include "audit.hpp"
#include "common_xml_document_parser.h"
#include "odf_ooxml_export.h"
#include "pimpl.h"
#include "safety_policy.h"
#include <concepts>

namespace df0
{

/**
 * @brief A parser for flat ODF XML documents.
 * @tparam safety_level The safety policy to use.
 * @tparam auditor_type The injected auditor type.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
class DF0_ODF_OOXML_EXPORT odfxml_parser
    : public common_xml_document_parser<safety_level, auditor_type>,
      public pipeline::transformer_element<odfxml_parser<safety_level, auditor_type>>,
      public with_pimpl<odfxml_parser<safety_level, auditor_type>>
{
	private:
		using base_type = common_xml_document_parser<safety_level, auditor_type>;
		using with_pimpl<odfxml_parser<safety_level, auditor_type>>::impl;
		friend pimpl_impl<odfxml_parser<safety_level, auditor_type>>;

		using base_type::registerODFOOXMLCommandHandler;
		using base_type::auditor;
		using scoped_context_stack_push = base_type::scoped_context_stack_push;

	protected:
		auto create_base_context_guard(const message_callbacks& emit_message)
		{
			return scoped_context_stack_push{*this, emit_message};
		}

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
		odfxml_parser()
			: odfxml_parser(audit::auditor_ref<stateless_auditor_type>{})
		{}

		explicit odfxml_parser(audit::auditor_ref<auditor_type> auditor);
		/**
		 * @brief Processes a message in the parsing chain.
		 * @return The continuation status.
		 */
		continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

// Deduction guide for CTAD
odfxml_parser() -> odfxml_parser<default_safety_level, audit::null_auditor>;

} // namespace df0

#endif
