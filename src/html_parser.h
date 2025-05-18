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

#ifndef DOCWIRE_HTML_PARSER_H
#define DOCWIRE_HTML_PARSER_H

#include "html_export.h"
#include "chain_element.h"
#include "pimpl.h"

namespace docwire
{
class DOCWIRE_HTML_EXPORT HTMLParser : public ChainElement, public with_pimpl<HTMLParser>
{
	private:
		using with_pimpl<HTMLParser>::impl;
		friend pimpl_impl<HTMLParser>;

	public:

		HTMLParser();
		continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;
		bool is_leaf() const override { return false; }
		///turns off charset decoding. It may be useful, if we want to decode data ourself (EML parser is an example).
		void skipCharsetDecoding();
};

} // namespace docwire

#endif
