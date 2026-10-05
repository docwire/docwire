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

#include "html_exporter.h"

#include "document_elements.h"
#include "error_tags.h"
#include "html_writer.h"
#include "audit.hpp"
#include "audit/legacy_log_audit.hpp"
#include "serialization_message.h" // IWYU pragma: keep
#include "throw_if.h"
#include <sstream>

namespace docwire
{

template<typename auditor_type>
struct pimpl_impl<html_exporter<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
	using audit::with_auditor<auditor_type>::auditor;

	explicit pimpl_impl(html_exporter<auditor_type>& owner)
		: audit::with_auditor<auditor_type>{owner.auditor()}
	{}

	std::shared_ptr<std::stringstream> m_stream;
	html_writer m_writer;
	int m_nested_docs_level { 0 };
};

template<typename auditor_type>
html_exporter<auditor_type>::html_exporter(audit::auditor_ref<auditor_type> auditor)
	: audit::with_auditor<auditor_type>(auditor)
{}

template<typename auditor_type>
continuation html_exporter<auditor_type>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};
	if (msg->is<std::exception_ptr>())
		return emit_message(std::move(msg));
	if (msg->is<document::document>() || !impl().m_stream)
	{
		++impl().m_nested_docs_level;
		if (impl().m_nested_docs_level == 1)
			impl().m_stream = std::make_shared<std::stringstream>();
	}
	impl().m_writer.write_to(msg, *impl().m_stream);
	if (msg->is<document::close_document>())
	{
		throw_if(impl().m_nested_docs_level <= 0, errors::program_logic{});
		--impl().m_nested_docs_level;
		if (impl().m_nested_docs_level == 0)
		{
			emit_message(data_source{seekable_stream_ptr{impl().m_stream}});
			impl().m_stream.reset();
		}
	}
	return continuation::proceed;
}

template class DOCWIRE_HTML_EXPORT html_exporter<docwire::audit::null_auditor>;
template class DOCWIRE_HTML_EXPORT html_exporter<docwire::audit::legacy_log_audit>;

} // namespace docwire
