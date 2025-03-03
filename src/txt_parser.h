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

#include "parser.h"
#include "plain_text_export.h"

namespace docwire
{

struct parse_paragraphs { bool v; };
struct parse_lines { bool v; };

class DOCWIRE_PLAIN_TEXT_EXPORT TXTParser : public Parser, public with_pimpl<TXTParser>
{
	public:

	TXTParser(
		parse_paragraphs parse_paragraphs_arg = parse_paragraphs{true},
		parse_lines parse_lines_arg = parse_lines{true});

    void parse(const data_source& data) override;

		const std::vector<mime_type> supported_mime_types() override
		{
			return {
			mime_type{"text/x-asm"},
			mime_type{"text/asp"},
			mime_type{"text/aspdotnet"},
			mime_type{"text/x-basic"},
			mime_type{"text/x-bat"},
			mime_type{"text/x-c"},
			mime_type{"text/x-cmake"},
			mime_type{"text/x-csharp"},
			mime_type{"text/css"},
			mime_type{"text/csv"},
			mime_type{"text/x-d"},
			mime_type{"text/x-fortran"},
			mime_type{"text/x-fsharp"},
			mime_type{"text/x-go"},
			mime_type{"text/x-c++hdr"},
			mime_type{"text/html"},
			mime_type{"text/x-java-source"},
			mime_type{"application/javascript"},
			mime_type{"text/javascript"},
			mime_type{"application/json"},
			mime_type{"text/x-jsp"},
			mime_type{"text/x-lua"},
			mime_type{"text/markdown"},
			mime_type{"text/x-pascal"},
			mime_type{"application/x-httpd-php"},
			mime_type{"text/x-perl"},
			mime_type{"text/x-python"},
			mime_type{"text/x-rsrc"},
			mime_type{"application/rss+xml"},
			mime_type{"application/x-sh"},
			mime_type{"application/x-tcl"},
			mime_type{"text/plain"}, // !
			mime_type{"text/x-vbdotnet"},
			mime_type{"text/x-vbscript"},
			mime_type{"application/xml"},
			mime_type{"text/yaml"}
			};
		};

private:
	using with_pimpl<TXTParser>::impl;
};

} // namespace docwire

#endif
