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

#include <string>

#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class DllExport TXTParser : public Parser
{
	private:
		struct Implementation;
		Implementation* impl;

	public:

    void parse(const data_source& data) const override;
	bool understands(const data_source& data) const override;

		inline static const std::vector<mime_type> supported_mime_types =
		{
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

		TXTParser();
	static constexpr bool is_universal = true;
};

} // namespace docwire

#endif
