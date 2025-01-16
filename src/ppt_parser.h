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

#ifndef DOCWIRE_PPT_PARSER_H
#define DOCWIRE_PPT_PARSER_H

#include "parser.h"
#include "tags.h"
#include <vector>

namespace docwire
{

struct Metadata;
class ThreadSafeOLEStorage;

class DllExport PPTParser : public Parser
{
	private:
		attributes::Metadata metaData(const std::unique_ptr<ThreadSafeOLEStorage>& storage) const;

	public:
		PPTParser();
		const std::vector<mime_type> supported_mime_types() override
		{
			return {
			mime_type{"application/vnd.ms-powerpoint"},
			mime_type{"application/vnd.ms-powerpoint.presentation.macroenabled.12"},
			mime_type{"application/vnd.ms-powerpoint.template.macroenabled.12"},
			mime_type{"application/vnd.ms-powerpoint.slideshow.macroenabled.12"}
			};
		};
		void parse(const data_source& data) override;
};

} // namespace docwire

#endif
