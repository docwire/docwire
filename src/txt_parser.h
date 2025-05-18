/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_TXT_PARSER_H
#define DOCWIRE_TXT_PARSER_H

#include "chain_element.h"
#include "plain_text_export.h"

namespace docwire
{

struct parse_paragraphs { bool v; };
struct parse_lines { bool v; };

class DOCWIRE_PLAIN_TEXT_EXPORT TXTParser : public ChainElement, public with_pimpl<TXTParser>
{
	public:

	TXTParser(
		parse_paragraphs parse_paragraphs_arg = parse_paragraphs{true},
		parse_lines parse_lines_arg = parse_lines{true});
    
    continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;
    bool is_leaf() const override { return false; }

private:
	using with_pimpl<TXTParser>::impl;
};

} // namespace docwire

#endif
