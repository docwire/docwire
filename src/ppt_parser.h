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

#ifndef DOCWIRE_PPT_PARSER_H
#define DOCWIRE_PPT_PARSER_H

#include "ole_office_formats_export.h"
#include "pipeline/element_base.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire
{

class thread_safe_ole_storage;

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OLE_OFFICE_FORMATS_EXPORT ppt_parser
    : public pipeline::transformer_element<ppt_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	explicit ppt_parser(audit::auditor_ref<auditor_type> auditor = {});

	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
	using audit::with_auditor<auditor_type>::auditor;
};

} // namespace docwire

#endif
