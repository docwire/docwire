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

#ifndef DOCWIRE_IWORK_PARSER_H
#define DOCWIRE_IWORK_PARSER_H

#include "parser.h"
#include <string>
#include "tags.h"
#include <vector>

namespace docwire
{

class Metadata;

class DllExport IWorkParser : public Parser
{
	private:
		struct Implementation;
		std::unique_ptr<Implementation> impl;
		attributes::Metadata metaData(std::shared_ptr<std::istream> stream) const;

	public:
		IWorkParser();
		~IWorkParser();
		inline static const std::vector<mime_type> supported_mime_types = 
		{
			mime_type{"application/vnd.apple.pages"},
			mime_type{"application/vnd.apple.numbers"},
			mime_type{"application/vnd.apple.keynote"},
			mime_type{"application/x-iwork-pages-sffpages"},
			mime_type{"application/x-iwork-numbers-sffnumbers"},
			mime_type{"application/x-iwork-keynote-sffkey"}
		};

		void parse(const data_source& data) const override;
};

} // namespace docwire

#endif
