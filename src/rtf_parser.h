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

#ifndef DOCWIRE_RTF_PARSER_H
#define DOCWIRE_RTF_PARSER_H

#include "parser.h"
#include "rtf_export.h"
#include "tags.h"
#include <vector>

namespace docwire
{

class DOCWIRE_RTF_EXPORT RTFParser : public Parser
{
	private:
		attributes::Metadata metaData(const data_source& data) const;

	public:

    	void parse(const data_source& data) override;

		const std::vector<mime_type> supported_mime_types() override
		{
			return {
			mime_type{"application/rtf"},
			mime_type{"text/rtf"},
			mime_type{"text/richtext"}
			};
		};

		RTFParser();
};

} // namespace docwire

#endif
