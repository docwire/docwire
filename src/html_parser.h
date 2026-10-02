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

#ifndef DOCWIRE_HTML_PARSER_H
#define DOCWIRE_HTML_PARSER_H

#include "html_export.h"
#include "audit.hpp"
#include "pipeline/element_base.hpp"
#include "pimpl.h"

namespace docwire
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_HTML_EXPORT html_parser
    : public pipeline::transformer_element<html_parser>,
      public with_pimpl<html_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
	private:
		using with_pimpl<html_parser<auditor_type>>::impl;
		using audit::with_auditor<auditor_type>::auditor;
		friend pimpl_impl<html_parser>;

	public:

		explicit html_parser(audit::auditor_ref<auditor_type> auditor = {});
		continuation operator()(message_ptr msg, const message_callbacks& emit_message);

		///turns off charset decoding. It may be useful, if we want to decode data ourself (EML parser is an example).
		void skipCharsetDecoding();
};

} // namespace docwire

#endif
