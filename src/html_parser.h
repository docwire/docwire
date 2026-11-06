/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_HTML_PARSER_H
#define DF0_HTML_PARSER_H

#include "html_export.h"
#include "audit.hpp"
#include "pipeline/element_base.hpp"
#include "pimpl.h"
#include <concepts>

namespace df0
{

template<typename auditor_type = audit::null_auditor>
class DF0_HTML_EXPORT html_parser
    : public pipeline::transformer_element<html_parser<auditor_type>>,
      public with_pimpl<html_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
	private:
		using with_pimpl<html_parser<auditor_type>>::impl;
		using audit::with_auditor<auditor_type>::auditor;
		friend pimpl_impl<html_parser>;

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
		html_parser()
			: html_parser(audit::auditor_ref<stateless_auditor_type>{})
		{}

		explicit html_parser(audit::auditor_ref<auditor_type> auditor);
		continuation operator()(message_ptr msg, const message_callbacks& emit_message);

		///turns off charset decoding. It may be useful, if we want to decode data ourself (EML parser is an example).
		void skipCharsetDecoding();
};

// Deduction guide for CTAD
html_parser() -> html_parser<audit::null_auditor>;

} // namespace df0

#endif
