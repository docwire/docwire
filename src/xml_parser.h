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
#include "chain_element.h"
#include "xml_export.h"

namespace docwire
{

/**
 * @brief A parser for generic XML documents.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level>
class DOCWIRE_XML_EXPORT xml_parser : public chain_element
{
public:
	/**
	 * @brief Processes a message in the parsing chain.
	 * @return The continuation status.
	 */
	continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
	bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif // DOCWIRE_XML_PARSER_H
