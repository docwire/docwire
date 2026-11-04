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

#ifndef DOCWIRE_ARCHIVES_PARSER_H
#define DOCWIRE_ARCHIVES_PARSER_H

#include "archives_export.h"
#include "pipeline/element_base.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_ARCHIVES_EXPORT archives_parser
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

#endif // DOCWIRE_ARCHIVES_PARSER_H
