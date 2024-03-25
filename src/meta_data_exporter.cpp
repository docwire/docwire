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

#include "meta_data_exporter.h"

#include "meta_data_writer.h"
#include "parser.h"
#include <sstream>

namespace docwire
{

struct MetaDataExporter::Implementation
{
	std::shared_ptr<std::stringstream> m_stream;
	MetaDataWriter m_writer;
};

MetaDataExporter::MetaDataExporter()
	: impl(new Implementation)
{}

MetaDataExporter::MetaDataExporter(const MetaDataExporter& other)
	: impl(new Implementation(), ImplementationDeleter())
{
}

void MetaDataExporter::process(Info &info) const
{
	if (std::holds_alternative<tag::Document>(info.tag) || !impl->m_stream)
		impl->m_stream = std::make_shared<std::stringstream>();
	impl->m_writer.write_to(info.tag, *impl->m_stream);
	if (std::holds_alternative<tag::CloseDocument>(info.tag))
	{
		Info info(tag::File{impl->m_stream, std::string("")});
		emit(info);
		impl->m_stream.reset();
	}
}

void MetaDataExporter::ImplementationDeleter::operator()(MetaDataExporter::Implementation* impl)
{
	delete impl;
}

} // namespace docwire
