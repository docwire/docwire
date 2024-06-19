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

//#include "parser.h"
#include "plain_text_writer.h"
#include <sstream>

namespace docwire
{

struct PlainTextExporter::Implementation
{
	std::shared_ptr<std::stringstream> m_stream;
	PlainTextWriter m_writer;
	int m_nested_docs_level { 0 };
};

PlainTextExporter::PlainTextExporter()
	: impl(new Implementation)
{}

PlainTextExporter::PlainTextExporter(const PlainTextExporter& other)
	: impl(new Implementation(), ImplementationDeleter())
{
}

void PlainTextExporter::process(Info &info) const
{
	if (std::holds_alternative<tag::Document>(info.tag) || !impl->m_stream)
	{
		++impl->m_nested_docs_level;
		if (impl->m_nested_docs_level == 1)
			impl->m_stream = std::make_shared<std::stringstream>();
	}
	impl->m_writer.write_to(info.tag, *impl->m_stream);
	if (std::holds_alternative<tag::CloseDocument>(info.tag))
	{
		--impl->m_nested_docs_level;
		if (impl->m_nested_docs_level == 0)
		{
			Info info{data_source{seekable_stream_ptr{impl->m_stream}, file_extension{".txt"}}};
			emit(info);
			impl->m_stream.reset();
		}
	}
}

void PlainTextExporter::ImplementationDeleter::operator()(PlainTextExporter::Implementation* impl)
{
	delete impl;
}

} // namespace docwire
