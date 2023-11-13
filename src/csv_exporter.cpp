/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "csv_exporter.h"

#include "csv_writer.h"
#include "parser.h"
#include <sstream>

namespace docwire
{

namespace experimental
{

struct CsvExporter::Implementation
{
	std::stringstream m_stream;
	CsvWriter m_writer;
};

CsvExporter::CsvExporter()
	: impl(new Implementation)
{}

CsvExporter::CsvExporter(const CsvExporter& other)
	: impl(new Implementation(), ImplementationDeleter())
{
}

void CsvExporter::process(Info &info) const
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

void CsvExporter::ImplementationDeleter::operator()(CsvExporter::Implementation* impl)
{
	delete impl;
}

} // namespace experimental

} // namespace docwire
