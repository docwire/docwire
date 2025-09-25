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

#include "plain_text_exporter.h"

#include "document_elements.h"
#include "error_tags.h"
#include "plain_text_writer.h"
#include "throw_if.h"
#include <sstream>

namespace docwire
{

template<>
struct pimpl_impl<PlainTextExporter> : pimpl_impl_base
{
	pimpl_impl(eol_sequence eol_sequence, link_formatter link_formatter)
		: m_writer{eol_sequence.v, link_formatter.format_opening, link_formatter.format_closing}
	{}

	std::shared_ptr<std::stringstream> m_stream;
	PlainTextWriter m_writer;
	int m_nested_docs_level { 0 };
};

PlainTextExporter::PlainTextExporter(eol_sequence eol_sequence, link_formatter link_formatter)
	: with_pimpl<PlainTextExporter>(eol_sequence, link_formatter)
{}

continuation PlainTextExporter::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (msg->is<std::exception_ptr>())
		return emit_message(std::move(msg));
	if (msg->is<document::Document>() || !impl().m_stream)
	{
		++impl().m_nested_docs_level;
		if (impl().m_nested_docs_level == 1)
			impl().m_stream = std::make_shared<std::stringstream>();
	}
	impl().m_writer.write_to(msg, *impl().m_stream);
	if (msg->is<document::CloseDocument>())
	{
		throw_if(impl().m_nested_docs_level <= 0, errors::program_logic{});
		--impl().m_nested_docs_level;
		if (impl().m_nested_docs_level == 0)
		{
			emit_message(data_source{seekable_stream_ptr{impl().m_stream}, mime_type{"text/plain"}, confidence::highest});
			impl().m_stream.reset();
		}
	}
	return continuation::proceed;
}

} // namespace docwire
