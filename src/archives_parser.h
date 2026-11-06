/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_ARCHIVES_PARSER_H
#define DF0_ARCHIVES_PARSER_H

#include "archives_export.h"
#include "pipeline/element_base.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0
{

template<typename auditor_type = audit::null_auditor>
class DF0_ARCHIVES_EXPORT archives_parser
    : public pipeline::transformer_element<archives_parser<auditor_type>>,
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
	archives_parser()
		: archives_parser(audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit archives_parser(audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>(auditor)
	{}

	/**
	* @brief Executes transform operation for given node data.
	* @see df0::message_ptr
	* @param msg incoming message
	* @param emit_message callback to emit messages
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
	using audit::with_auditor<auditor_type>::auditor;
};

// Deduction guide for CTAD
archives_parser() -> archives_parser<audit::null_auditor>;

} // namespace df0

#endif // DF0_ARCHIVES_PARSER_H
