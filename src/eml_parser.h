/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_EML_PARSER_H
#define DF0_EML_PARSER_H

#include "mail_export.h"

#include "pipeline/element_base.hpp"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0
{

template<typename auditor_type = audit::null_auditor>
class DF0_MAIL_EXPORT eml_parser
    : public pipeline::transformer_element<eml_parser<auditor_type>>,
      public with_pimpl<eml_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
	private:
		using with_pimpl<eml_parser<auditor_type>>::impl;
		using audit::with_auditor<auditor_type>::auditor;
		friend pimpl_impl<eml_parser<auditor_type>>;

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
		eml_parser()
			: eml_parser(audit::auditor_ref<stateless_auditor_type>{})
		{}

		explicit eml_parser(audit::auditor_ref<auditor_type> auditor);

		continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

// Deduction guide for CTAD
eml_parser() -> eml_parser<audit::null_auditor>;

} // namespace df0

#endif
