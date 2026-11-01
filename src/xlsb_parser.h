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

#ifndef DOCWIRE_XLSB_PARSER_H
#define DOCWIRE_XLSB_PARSER_H

#include "pipeline/element_base.hpp"
#include "message.h"
#include "pimpl.h"
#include "xlsb_export.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace docwire
{

class zip_reader;

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_XLSB_EXPORT xlsb_parser
    : public pipeline::transformer_element<xlsb_parser<auditor_type>>,
      public with_pimpl<xlsb_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
private:
	friend pimpl_impl<xlsb_parser<auditor_type>>;
	using with_pimpl<xlsb_parser<auditor_type>>::impl;
	using audit::with_auditor<auditor_type>::auditor;

public:
	/**
	 * @brief Constructs the component with the default stateless auditing policy.
	 *
	 * This overload is available only when the configured auditor is stateless
	 * and therefore default-constructible.
	 *
	 * @tparam stateless_auditor_type Injected auditor type selected by this overload.
	 */
	template<audit::stateless_auditor stateless_auditor_type = auditor_type>
	xlsb_parser()
		: xlsb_parser(audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit xlsb_parser(audit::auditor_ref<auditor_type> auditor);

	continuation operator()(message_ptr msg, const message_callbacks& emit_message);
};

} // namespace docwire

#endif
