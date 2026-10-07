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

#ifndef DOCWIRE_TXT_PARSER_H
#define DOCWIRE_TXT_PARSER_H

#include "pipeline/element_base.hpp"
#include "plain_text_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire
{

struct parse_paragraphs { bool v; };
struct parse_lines { bool v; };

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_PLAIN_TEXT_EXPORT txt_parser
    : public pipeline::transformer_element<txt_parser<auditor_type>>,
      public with_pimpl<txt_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	explicit txt_parser(audit::auditor_ref<auditor_type> auditor);

	explicit txt_parser(
		parse_paragraphs parse_paragraphs_arg = parse_paragraphs{true},
		parse_lines parse_lines_arg = parse_lines{true},
		audit::auditor_ref<auditor_type> auditor = {});

	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
	using with_pimpl<txt_parser<auditor_type>>::impl;
	using audit::with_auditor<auditor_type>::auditor;
};

} // namespace docwire

#endif
