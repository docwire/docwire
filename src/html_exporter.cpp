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

#include "html_exporter.h"

#include "html_writer.h"
#include "parser.h"
#include <sstream>

namespace docwire
{

struct HtmlExporter::Implementation
{
	RestoreOriginalAttributes m_restore_original_attributes;
	std::stringstream m_stream;
	HtmlWriter m_writer;
	Implementation(RestoreOriginalAttributes restore_original_attributes)
		: m_restore_original_attributes(restore_original_attributes),
		m_writer(static_cast<HtmlWriter::RestoreOriginalAttributes>(m_restore_original_attributes))
	{}
};

HtmlExporter::HtmlExporter(RestoreOriginalAttributes restore_original_attributes)
  : impl(new Implementation(restore_original_attributes), ImplementationDeleter())
{}

HtmlExporter::HtmlExporter(const HtmlExporter& other)
	: impl(new Implementation(other.impl->m_restore_original_attributes), ImplementationDeleter())
{
}

void HtmlExporter::process(Info &info) const
{
	if (info.tag_name == StandardTag::TAG_DOCUMENT)
		impl->m_stream.clear();
	impl->m_writer.write_to(info, impl->m_stream);
	if (info.tag_name == StandardTag::TAG_CLOSE_DOCUMENT)
	{
		Info info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&impl->m_stream}, {"name", ""}});
		emit(info);
	}
}

void HtmlExporter::ImplementationDeleter::operator()(HtmlExporter::Implementation* impl)
{
	delete impl;
}

} // namespace docwire
