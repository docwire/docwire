/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_XML_PARSER_H
#define DF0_XML_PARSER_H

#include "safety_policy.h"
#include "audit.hpp"
#include "pipeline/element_base.hpp"
#include "xml_export.h"
#include <concepts>

namespace df0
{

/**
 * @brief A parser for generic XML documents.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
class DF0_XML_EXPORT xml_parser : public pipeline::transformer_element<xml_parser<safety_level, auditor_type>>, private audit::with_auditor<auditor_type>
{
    using audit::with_auditor<auditor_type>::auditor;
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
	xml_parser()
		: xml_parser(audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit xml_parser(audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>(auditor)
	{}

	/**
	 * @brief Processes a message in the parsing chain.
	 * @return The continuation status.
	 */
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

} // namespace df0

#endif // DF0_XML_PARSER_H
