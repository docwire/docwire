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
	std::stringstream m_stream;
	PlainTextWriter m_writer;
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
	if (info.tag_name == StandardTag::TAG_DOCUMENT)
		impl->m_stream.clear();
	impl->m_writer.write_to(info, impl->m_stream);
	if (info.tag_name == StandardTag::TAG_CLOSE_DOCUMENT)
	{
		Info info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&impl->m_stream}, {"name", std::string("plain_text_export.txt")}});
		emit(info);
	}
}

void PlainTextExporter::ImplementationDeleter::operator()(PlainTextExporter::Implementation* impl)
{
	delete impl;
}

} // namespace docwire
