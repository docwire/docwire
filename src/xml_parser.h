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

#ifndef DOCWIRE_XML_PARSER_H
#define DOCWIRE_XML_PARSER_H

#include "safety_policy.h"
#include "audit.hpp"
#include "pipeline/element_base.hpp"
#include "xml_export.h"

namespace docwire
{

/**
 * @brief A parser for generic XML documents.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level,
          typename auditor_type = audit::null_auditor>
class DOCWIRE_XML_EXPORT xml_parser : public pipeline::transformer_element<xml_parser<safety_level, auditor_type>>, private audit::with_auditor<auditor_type>
{
    using audit::with_auditor<auditor_type>::auditor;
public:
	explicit xml_parser(audit::auditor_ref<auditor_type> auditor = {})
		: audit::with_auditor<auditor_type>(auditor)
	{}

	/**
	 * @brief Processes a message in the parsing chain.
	 * @return The continuation status.
	 */
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

} // namespace docwire

#endif // DOCWIRE_XML_PARSER_H
