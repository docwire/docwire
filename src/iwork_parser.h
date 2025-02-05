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
#include "tags.h"
#include <vector>

namespace docwire
{

class Metadata;

class DOCWIRE_EXPORT IWorkParser : public Parser, public with_pimpl<IWorkParser>
{
	private:
		using with_pimpl<IWorkParser>::impl;
		using with_pimpl<IWorkParser>::renew_impl;
		attributes::Metadata metaData(std::shared_ptr<std::istream> stream) const;

	public:
		IWorkParser();
		const std::vector<mime_type> supported_mime_types() override
		{
			return {
			mime_type{"application/vnd.apple.pages"},
			mime_type{"application/vnd.apple.numbers"},
			mime_type{"application/vnd.apple.keynote"},
			mime_type{"application/x-iwork-pages-sffpages"},
			mime_type{"application/x-iwork-numbers-sffnumbers"},
			mime_type{"application/x-iwork-keynote-sffkey"}
			};
		};

		void parse(const data_source& data) override;
};

} // namespace docwire

#endif
